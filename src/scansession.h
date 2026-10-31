// The document being scanned ("Session" in QML): its pages, the processing
// of each photo into a page, and the export to PDF. Image work and OCR run
// in a worker thread, one job at a time; "busy" is set meanwhile.
//
// Each page keeps its source photo and settings, so it can be edited again.
// Photos and processed pages live in ~/.cache/<package name>, which is
// emptied when the document is saved or discarded and on every start.

#ifndef SCANSESSION_H
#define SCANSESSION_H

#include <QObject>
#include <QStringList>
#include <QVariantList>
#include <QVector>

#include <functional>

class ScanSession : public QObject
{
    Q_OBJECT
    // File URLs of the processed page images, in document order.
    Q_PROPERTY(QStringList pages READ pages NOTIFY pagesChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    // Pages written so far while exporting.
    Q_PROPERTY(int exportedPages READ exportedPages NOTIFY exportedPagesChanged)

public:
    explicit ScanSession(QObject *parent = nullptr);

    QStringList pages() const;
    bool busy() const { return m_busy; }
    int exportedPages() const { return m_exportedPages; }

    // Where the camera should store the next photo.
    Q_INVOKABLE QString newCapturePath() const;

    // corners: x0, y0, ... x3, y3 normalised to 0..1, clockwise from top left.
    // index -1 adds a page, otherwise that page is replaced.
    Q_INVOKABLE void processPage(const QString &sourceUrl, const QVariantList &corners,
                                 int filter, int index);
    // { source, corners, filter } of a page, to edit it again.
    Q_INVOKABLE QVariantMap pageSettings(int index) const;
    Q_INVOKABLE void rotatePage(int index);
    Q_INVOKABLE void removePage(int index);
    Q_INVOKABLE void movePage(int from, int to);
    Q_INVOKABLE void clear();

    // ocrLanguages "" writes the images without text recognition.
    Q_INVOKABLE void exportPdf(const QString &title, const QString &ocrLanguages);

signals:
    void pagesChanged();
    void busyChanged();
    void exportedPagesChanged();
    void pageReady(int index);
    void exportFinished(const QString &url);
    void failed(const QString &message);

private:
    struct Page {
        QString source;
        QVariantList corners;
        int filter = 0;
        int rotation = 0; // quarter turns clockwise
        QString image;
    };

    using Job = std::function<QString()>;
    using Done = std::function<void(const QString &error)>;

    void runJob(const Job &job, const Done &done);
    void setBusy(bool busy);
    void setExportedPages(int pages);
    void storePage(const Page &page, int index);
    QString nextImagePath();
    bool isValidIndex(int index) const;

    static QString cacheRoot();
    static bool isOwnFile(const QString &path);

    QVector<Page> m_pages;
    bool m_busy = false;
    int m_exportedPages = 0;
    int m_serial = 0;
};

#endif
