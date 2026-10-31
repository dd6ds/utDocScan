// User interface language ("Language" in QML). By default the app follows
// the language selected on the phone; the user can pick a fixed one in the
// settings, which is stored and applied immediately (QQmlEngine::retranslate).
// Also lists the OCR languages shipped in the click.

#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QObject>
#include <QTranslator>
#include <QVariantList>

class QQmlEngine;

class LanguageManager : public QObject
{
    Q_OBJECT
    // "" = like the phone, otherwise a code from uiLanguages ("de", "fr", ...).
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    // The language actually shown, also when following the phone.
    Q_PROPERTY(QString effectiveLanguage READ effectiveLanguage NOTIFY languageChanged)
    // [{ code, name }] with each language named in itself.
    Q_PROPERTY(QVariantList uiLanguages READ uiLanguages CONSTANT)
    // [{ code, name }] with Tesseract codes ("deu") and the name in the UI language.
    Q_PROPERTY(QVariantList ocrLanguages READ ocrLanguages NOTIFY languageChanged)

public:
    explicit LanguageManager(QQmlEngine *engine, QObject *parent = nullptr);

    QString language() const { return m_language; }
    void setLanguage(const QString &code);
    QString effectiveLanguage() const;
    QVariantList uiLanguages() const;
    QVariantList ocrLanguages() const;

    // OCR languages for a first start: the UI language plus English, e.g. "deu+eng".
    Q_INVOKABLE QString defaultOcrLanguages() const;

signals:
    void languageChanged();

private:
    void applyTranslation();

    QQmlEngine *m_engine;
    QTranslator m_appTranslator;
    QTranslator m_qtTranslator;
    QString m_language;
};

#endif
