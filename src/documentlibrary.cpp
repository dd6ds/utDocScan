// Saved PDF documents, see documentlibrary.h.

#include "documentlibrary.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QUrl>

namespace {

const QString kPdfSuffix = QStringLiteral(".pdf");
const int kMaxNameLength = 80;

// File names keep letters of every script, digits, spaces and a few
// punctuation marks; anything else (slashes in particular) becomes "_".
QString safeFileName(const QString &name)
{
    static const QRegularExpression unsafe(QStringLiteral("[^\\w \\-.,()]"),
                                           QRegularExpression::UseUnicodePropertiesOption);
    QString result = name.trimmed();
    result.replace(unsafe, QStringLiteral("_"));
    while (result.startsWith(QLatin1Char('.')))
        result.remove(0, 1);
    return result.left(kMaxNameLength).trimmed();
}

QString localPath(const QString &url)
{
    return QUrl(url).toLocalFile();
}

// Only files inside the documents folder may be renamed or deleted.
bool isDocument(const QString &path)
{
    return !path.isEmpty() && QFileInfo(path).absolutePath() == DocumentLibrary::documentsDir();
}

} // namespace

DocumentLibrary::DocumentLibrary(QObject *parent) : QObject(parent)
{
    refresh();
}

// ~/Documents, where the file manager and other apps find the PDFs. A
// confined app may not write there by default; ubdocscan.apparmor grants it
// for this one folder (write_path).
QString DocumentLibrary::documentsDir()
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QDir().mkpath(dir);
    return dir;
}

QString DocumentLibrary::newPdfPath(const QString &title)
{
    QString base = safeFileName(title);
    if (base.isEmpty())
        base = QStringLiteral("Scan");
    const QDir dir(documentsDir());
    QString path = dir.filePath(base + kPdfSuffix);
    for (int n = 2; QFileInfo::exists(path); ++n)
        path = dir.filePath(QStringLiteral("%1 (%2)%3").arg(base).arg(n).arg(kPdfSuffix));
    return path;
}

void DocumentLibrary::refresh()
{
    const QFileInfoList files = QDir(documentsDir()).entryInfoList(
        { QStringLiteral("*") + kPdfSuffix }, QDir::Files, QDir::Time);
    QVariantList documents;
    for (const QFileInfo &file : files) {
        QVariantMap document;
        document.insert(QStringLiteral("name"), file.completeBaseName());
        document.insert(QStringLiteral("url"),
                        QUrl::fromLocalFile(file.absoluteFilePath()).toString());
        document.insert(QStringLiteral("size"), file.size());
        document.insert(QStringLiteral("modified"), file.lastModified());
        documents.append(document);
    }
    m_documents = documents;
    emit documentsChanged();
}

bool DocumentLibrary::rename(const QString &url, const QString &newName)
{
    const QString path = localPath(url);
    const QString name = safeFileName(newName);
    if (!isDocument(path) || name.isEmpty())
        return false;
    const QString target = QDir(documentsDir()).filePath(name + kPdfSuffix);
    if (target == path)
        return true;
    if (QFileInfo::exists(target) || !QFile::rename(path, target))
        return false;
    refresh();
    return true;
}

bool DocumentLibrary::remove(const QString &url)
{
    const QString path = localPath(url);
    if (!isDocument(path) || !QFile::remove(path))
        return false;
    refresh();
    return true;
}
