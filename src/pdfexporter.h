// Writes the scanned pages as a PDF: with OCR a searchable PDF (page image
// plus an invisible text layer from Tesseract), without OCR the images only.
// Blocking; ScanSession runs it in a worker thread.

#ifndef PDFEXPORTER_H
#define PDFEXPORTER_H

#include <QCoreApplication>
#include <QSize>
#include <QString>
#include <QStringList>

#include <functional>

class PdfExporter
{
    Q_DECLARE_TR_FUNCTIONS(PdfExporter)

public:
    // Called after each page with the number of pages done.
    using Progress = std::function<void(int done)>;

    PdfExporter(const QStringList &imagePaths, const QString &pdfPath, const QString &title);

    // ocrLanguages: Tesseract codes joined with "+", e.g. "deu+eng".
    // Both return an error text, or an empty string on success.
    QString writeSearchable(const QString &ocrLanguages, const Progress &progress) const;
    QString writeImagesOnly(const Progress &progress) const;

private:
    static QString tessdataDir();
    static int pageResolution(const QSize &pixels);

    QStringList m_imagePaths;
    QString m_pdfPath;
    QString m_title;
};

#endif
