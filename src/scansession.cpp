// Pages of the current scan and the jobs that make and export them, see
// scansession.h.

#include "scansession.h"

#include "documentlibrary.h"
#include "pageprocessor.h"
#include "pdfexporter.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFutureWatcher>
#include <QImage>
#include <QPolygonF>
#include <QStandardPaths>
#include <QUrl>
#include <QtConcurrent>

namespace {

// A4 at 300 dpi: enough for OCR, small enough to keep PDFs reasonable.
const int kMaxPageSide = 3508;
const int kJpegQuality = 85;
const int kCornerValues = 8;

QString toLocalPath(const QString &url)
{
    return QUrl(url).isLocalFile() ? QUrl(url).toLocalFile() : url;
}

QString processImage(const QString &source, const QVariantList &corners, int filter,
                     int rotation, const QString &target)
{
    const QImage photo = PageProcessor::loadImage(source);
    if (photo.isNull())
        return ScanSession::tr("The photo could not be opened.");
    QPolygonF quad;
    for (int i = 0; i < kCornerValues; i += 2)
        quad << QPointF(corners.at(i).toDouble(), corners.at(i + 1).toDouble());
    QImage page = PageProcessor::warp(photo, quad, kMaxPageSide);
    for (int turn = 0; turn < rotation; ++turn)
        page = PageProcessor::rotateClockwise(page);
    page = PageProcessor::applyFilter(page, PageProcessor::Filter(filter));
    if (!page.save(target, "JPG", kJpegQuality))
        return ScanSession::tr("The page could not be saved.");
    return QString();
}

} // namespace

ScanSession::ScanSession(QObject *parent) : QObject(parent)
{
    // Leftovers of a run that ended without saving or discarding.
    QDir(cacheRoot()).removeRecursively();
}

// Confined apps may only write below ~/.cache/<package name>.
QString ScanSession::cacheRoot()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)
           + QLatin1Char('/') + QCoreApplication::organizationName();
}

// Photos taken by the app or handed over by the Content Hub (which copies
// them to ~/.local/share/<package name>/HubIncoming) belong to the app and
// are deleted with the session; a picked file elsewhere is not.
bool ScanSession::isOwnFile(const QString &path)
{
    const QString hubIncoming = QStandardPaths::writableLocation(
                                    QStandardPaths::GenericDataLocation)
                                + QLatin1Char('/') + QCoreApplication::organizationName()
                                + QStringLiteral("/HubIncoming/");
    return path.startsWith(cacheRoot() + QLatin1Char('/')) || path.startsWith(hubIncoming);
}

QStringList ScanSession::pages() const
{
    QStringList urls;
    for (const Page &page : m_pages)
        urls << QUrl::fromLocalFile(page.image).toString();
    return urls;
}

QString ScanSession::newCapturePath() const
{
    const QString dir = cacheRoot() + QStringLiteral("/captures");
    QDir().mkpath(dir);
    return dir + QStringLiteral("/photo-%1.jpg").arg(QDateTime::currentMSecsSinceEpoch());
}

// Every version of a page gets a new file name, so QML never shows a cached
// older image.
QString ScanSession::nextImagePath()
{
    const QString dir = cacheRoot() + QStringLiteral("/pages");
    QDir().mkpath(dir);
    return dir + QStringLiteral("/page-%1.jpg").arg(++m_serial);
}

bool ScanSession::isValidIndex(int index) const
{
    return index >= 0 && index < m_pages.size();
}

void ScanSession::setBusy(bool busy)
{
    if (busy == m_busy)
        return;
    m_busy = busy;
    emit busyChanged();
}

void ScanSession::setExportedPages(int pages)
{
    if (pages == m_exportedPages)
        return;
    m_exportedPages = pages;
    emit exportedPagesChanged();
}

void ScanSession::runJob(const Job &job, const Done &done)
{
    setBusy(true);
    auto *watcher = new QFutureWatcher<QString>(this);
    connect(watcher, &QFutureWatcher<QString>::finished, this, [this, watcher, done] {
        watcher->deleteLater();
        setBusy(false);
        done(watcher->result());
    });
    watcher->setFuture(QtConcurrent::run(job));
}

void ScanSession::processPage(const QString &sourceUrl, const QVariantList &corners, int filter,
                              int index)
{
    if (m_busy || corners.size() != kCornerValues || (index != -1 && !isValidIndex(index)))
        return;
    Page page;
    page.source = toLocalPath(sourceUrl);
    page.corners = corners;
    page.filter = filter;
    page.rotation = index == -1 ? 0 : m_pages.at(index).rotation;
    page.image = nextImagePath();
    runJob([page] {
        return processImage(page.source, page.corners, page.filter, page.rotation, page.image);
    }, [this, page, index](const QString &error) {
        if (!error.isEmpty()) {
            emit failed(error);
            return;
        }
        storePage(page, index);
    });
}

void ScanSession::storePage(const Page &page, int index)
{
    if (index == -1) {
        m_pages.append(page);
        index = m_pages.size() - 1;
    } else {
        // The source stays: it is the same photo, edited again.
        QFile::remove(m_pages.at(index).image);
        m_pages[index] = page;
    }
    emit pagesChanged();
    emit pageReady(index);
}

QVariantMap ScanSession::pageSettings(int index) const
{
    if (!isValidIndex(index))
        return {};
    const Page &page = m_pages.at(index);
    return { { QStringLiteral("source"), QUrl::fromLocalFile(page.source).toString() },
             { QStringLiteral("corners"), page.corners },
             { QStringLiteral("filter"), page.filter } };
}

void ScanSession::rotatePage(int index)
{
    if (m_busy || !isValidIndex(index))
        return;
    const QString source = m_pages.at(index).image;
    const QString target = nextImagePath();
    runJob([source, target] {
        const QImage rotated = PageProcessor::rotateClockwise(QImage(source));
        return rotated.save(target, "JPG", kJpegQuality) ? QString()
                                                         : tr("The page could not be saved.");
    }, [this, index, source, target](const QString &error) {
        if (!error.isEmpty() || !isValidIndex(index) || m_pages.at(index).image != source) {
            QFile::remove(target);
            if (!error.isEmpty())
                emit failed(error);
            return;
        }
        QFile::remove(source);
        m_pages[index].image = target;
        m_pages[index].rotation = (m_pages.at(index).rotation + 1) % 4;
        emit pagesChanged();
    });
}

void ScanSession::removePage(int index)
{
    if (m_busy || !isValidIndex(index))
        return;
    const Page page = m_pages.takeAt(index);
    QFile::remove(page.image);
    // A photo can be the source of one page only, so it can go too.
    if (isOwnFile(page.source))
        QFile::remove(page.source);
    emit pagesChanged();
}

void ScanSession::movePage(int from, int to)
{
    if (m_busy || !isValidIndex(from) || !isValidIndex(to) || from == to)
        return;
    m_pages.move(from, to);
    emit pagesChanged();
}

void ScanSession::clear()
{
    if (m_busy)
        return;
    for (const Page &page : qAsConst(m_pages)) {
        if (isOwnFile(page.source))
            QFile::remove(page.source);
    }
    m_pages.clear();
    QDir(cacheRoot()).removeRecursively();
    emit pagesChanged();
}

void ScanSession::exportPdf(const QString &title, const QString &ocrLanguages)
{
    if (m_busy || m_pages.isEmpty())
        return;
    QStringList images;
    for (const Page &page : qAsConst(m_pages))
        images << page.image;
    const QString pdfPath = DocumentLibrary::newPdfPath(title);
    setExportedPages(0);
    qDebug() << "ScanSession: exporting" << images.size() << "pages to" << pdfPath
             << "OCR:" << (ocrLanguages.isEmpty() ? QStringLiteral("off") : ocrLanguages);

    runJob([this, images, pdfPath, title, ocrLanguages] {
        const PdfExporter exporter(images, pdfPath, title);
        const auto progress = [this](int done) {
            QMetaObject::invokeMethod(this, [this, done] { setExportedPages(done); },
                                      Qt::QueuedConnection);
        };
        return ocrLanguages.isEmpty() ? exporter.writeImagesOnly(progress)
                                      : exporter.writeSearchable(ocrLanguages, progress);
    }, [this, pdfPath](const QString &error) {
        if (!error.isEmpty()) {
            QFile::remove(pdfPath);
            emit failed(error);
            return;
        }
        emit exportFinished(QUrl::fromLocalFile(pdfPath).toString());
    });
}
