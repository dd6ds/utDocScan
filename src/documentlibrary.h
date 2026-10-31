// The saved PDFs ("Library" in QML): listing, renaming and deleting the
// PDF files in ~/Documents.

#ifndef DOCUMENTLIBRARY_H
#define DOCUMENTLIBRARY_H

#include <QObject>
#include <QVariantList>

class DocumentLibrary : public QObject
{
    Q_OBJECT
    // Newest first: [{ name, url, size (bytes), modified (date) }].
    Q_PROPERTY(QVariantList documents READ documents NOTIFY documentsChanged)

public:
    explicit DocumentLibrary(QObject *parent = nullptr);

    QVariantList documents() const { return m_documents; }

    Q_INVOKABLE void refresh();
    Q_INVOKABLE bool rename(const QString &url, const QString &newName);
    Q_INVOKABLE bool remove(const QString &url);

    static QString documentsDir();
    // A path for a new PDF named after the title, not overwriting an existing one.
    static QString newPdfPath(const QString &title);

signals:
    void documentsChanged();

private:
    QVariantList m_documents;
};

#endif
