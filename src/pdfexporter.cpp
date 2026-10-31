// PDF output with and without OCR, see pdfexporter.h.

#include "pdfexporter.h"

#include <QCoreApplication>
#include <QDebug>
#include <QImage>
#include <QPageSize>
#include <QPainter>
#include <QPdfWriter>

#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include <tesseract/renderer.h>

#include <algorithm>
#include <memory>

namespace {

// Pages are sized like A4 (8.27 x 11.69 in): the resolution is chosen so the
// image just fits, which also tells Tesseract the real text size.
const double kA4WidthInch = 8.27;
const double kA4HeightInch = 11.69;
const double kPdfPointsPerInch = 72.0;

} // namespace

PdfExporter::PdfExporter(const QStringList &imagePaths, const QString &pdfPath,
                         const QString &title)
    : m_imagePaths(imagePaths), m_pdfPath(pdfPath), m_title(title)
{
}

// The language models and pdf.ttf are installed into the click next to the
// binary (CMakeLists.txt).
QString PdfExporter::tessdataDir()
{
    return QCoreApplication::applicationDirPath() + QStringLiteral("/share/tessdata");
}

int PdfExporter::pageResolution(const QSize &pixels)
{
    const double portrait = std::max(pixels.width() / kA4WidthInch,
                                     pixels.height() / kA4HeightInch);
    const double landscape = std::max(pixels.width() / kA4HeightInch,
                                      pixels.height() / kA4WidthInch);
    const bool isLandscape = pixels.width() > pixels.height();
    return std::max(1, qRound(isLandscape ? landscape : portrait));
}

QString PdfExporter::writeSearchable(const QString &ocrLanguages,
                                     const Progress &progress) const
{
    const QByteArray dataDir = tessdataDir().toLocal8Bit();
    tesseract::TessBaseAPI api;
    if (api.Init(dataDir.constData(), ocrLanguages.toLatin1().constData(),
                 tesseract::OEM_LSTM_ONLY) != 0)
        return tr("The text recognition could not be started (%1).").arg(ocrLanguages);
    api.SetPageSegMode(tesseract::PSM_AUTO);

    // The renderer appends ".pdf" to the name it is given.
    QString base = m_pdfPath;
    base.chop(int(qstrlen(".pdf")));
    const QByteArray outputBase = base.toLocal8Bit();
    auto renderer = std::make_unique<tesseract::TessPDFRenderer>(outputBase.constData(),
                                                                 dataDir.constData(), false);
    if (!renderer->happy() || !renderer->BeginDocument(m_title.toUtf8().constData()))
        return tr("The PDF file could not be created.");

    for (int i = 0; i < m_imagePaths.size(); ++i) {
        const QByteArray path = m_imagePaths.at(i).toLocal8Bit();
        Pix *pix = pixRead(path.constData());
        if (!pix)
            return tr("Page %1 could not be read.").arg(i + 1);
        const int ppi = pageResolution(QSize(int(pixGetWidth(pix)), int(pixGetHeight(pix))));
        pixSetResolution(pix, ppi, ppi);
        // Passing the file name lets the renderer embed the JPEG as it is
        // instead of re-encoding the page.
        const bool ok = api.ProcessPage(pix, i, path.constData(), nullptr, 0, renderer.get());
        pixDestroy(&pix);
        if (!ok)
            return tr("The text on page %1 could not be recognised.").arg(i + 1);
        progress(i + 1);
    }
    if (!renderer->EndDocument())
        return tr("The PDF file could not be written.");
    api.End();
    return QString();
}

QString PdfExporter::writeImagesOnly(const Progress &progress) const
{
    QPdfWriter writer(m_pdfPath);
    writer.setTitle(m_title);
    writer.setCreator(QStringLiteral("ubDocScan"));
    writer.setPageMargins(QMarginsF(0, 0, 0, 0));
    QPainter painter;

    for (int i = 0; i < m_imagePaths.size(); ++i) {
        const QImage image(m_imagePaths.at(i));
        if (image.isNull())
            return tr("Page %1 could not be read.").arg(i + 1);
        const double ppi = pageResolution(image.size());
        writer.setPageSize(QPageSize(QSizeF(image.width() * kPdfPointsPerInch / ppi,
                                            image.height() * kPdfPointsPerInch / ppi),
                                     QPageSize::Point));
        // The first page starts with begin(), every further one with newPage().
        const bool started = i == 0 ? painter.begin(&writer) : writer.newPage();
        if (!started)
            return tr("The PDF file could not be created.");
        painter.drawImage(QRect(0, 0, writer.width(), writer.height()), image);
        progress(i + 1);
    }
    if (!painter.end())
        return tr("The PDF file could not be written.");
    return QString();
}
