// UI language selection and the list of OCR languages, see languagemanager.h.

#include "languagemanager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QLibraryInfo>
#include <QLocale>
#include <QQmlEngine>
#include <QSettings>

namespace {

const QString kSettingsKey = QStringLiteral("language");
const QString kSourceLanguage = QStringLiteral("en");

struct Language {
    const char *code;       // UI language, matches i18n/ubdocscan_<code>.ts
    const char *nativeName; // shown untranslated, so everyone finds their own
    const char *ocrCode;    // Tesseract model shipped in share/tessdata
    const char *ocrName;    // translated via QT_TRANSLATE_NOOP
};

// Every language has both a translation and an OCR model; CMakeLists.txt
// installs the models listed here.
const Language kLanguages[] = {
    { "en", "English",  "eng", QT_TRANSLATE_NOOP("LanguageManager", "English") },
    { "de", "Deutsch",  "deu", QT_TRANSLATE_NOOP("LanguageManager", "German") },
    { "fr", "Français", "fra", QT_TRANSLATE_NOOP("LanguageManager", "French") },
    { "es", "Español",  "spa", QT_TRANSLATE_NOOP("LanguageManager", "Spanish") },
    { "it", "Italiano", "ita", QT_TRANSLATE_NOOP("LanguageManager", "Italian") },
};

const Language *findLanguage(const QString &code)
{
    for (const Language &language : kLanguages) {
        if (code == QLatin1String(language.code))
            return &language;
    }
    return nullptr;
}

} // namespace

LanguageManager::LanguageManager(QQmlEngine *engine, QObject *parent)
    : QObject(parent), m_engine(engine)
{
    const QString stored = QSettings().value(kSettingsKey).toString();
    m_language = findLanguage(stored) ? stored : QString();
    applyTranslation();
}

void LanguageManager::setLanguage(const QString &code)
{
    const QString language = findLanguage(code) ? code : QString();
    if (language == m_language)
        return;
    m_language = language;
    QSettings().setValue(kSettingsKey, m_language);
    applyTranslation();
    m_engine->retranslate();
    emit languageChanged();
}

QString LanguageManager::effectiveLanguage() const
{
    if (!m_language.isEmpty())
        return m_language;
    // QLocale::system() reads LANG/LANGUAGE, which Lomiri sets from the
    // phone's language setting.
    const QString system = QLocale::system().name().left(2);
    return findLanguage(system) ? system : kSourceLanguage;
}

QVariantList LanguageManager::uiLanguages() const
{
    QVariantList list;
    for (const Language &language : kLanguages) {
        list.append(QVariantMap{ { QStringLiteral("code"), QString::fromLatin1(language.code) },
                                 { QStringLiteral("name"), QString::fromUtf8(language.nativeName) } });
    }
    return list;
}

QVariantList LanguageManager::ocrLanguages() const
{
    QVariantList list;
    for (const Language &language : kLanguages) {
        list.append(QVariantMap{
            { QStringLiteral("code"), QString::fromLatin1(language.ocrCode) },
            { QStringLiteral("name"), QCoreApplication::translate("LanguageManager",
                                                                   language.ocrName) } });
    }
    return list;
}

QString LanguageManager::defaultOcrLanguages() const
{
    const QString ocr = QString::fromLatin1(findLanguage(effectiveLanguage())->ocrCode);
    return ocr == QLatin1String("eng") ? ocr : ocr + QStringLiteral("+eng");
}

// English is the source language, so a text missing from a translation stays
// English. English has a translation file too, for its plural forms ("%n pages").
void LanguageManager::applyTranslation()
{
    QCoreApplication::removeTranslator(&m_appTranslator);
    QCoreApplication::removeTranslator(&m_qtTranslator);
    const QString code = effectiveLanguage();
    if (m_appTranslator.load(QStringLiteral(":/i18n/ubdocscan_") + code))
        QCoreApplication::installTranslator(&m_appTranslator);
    // Qt's own texts (dialog buttons), if the phone has them installed.
    if (m_qtTranslator.load(QStringLiteral("qtbase_") + code,
                            QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        QCoreApplication::installTranslator(&m_qtTranslator);
    qDebug() << "LanguageManager:" << code << (m_language.isEmpty() ? "(phone)" : "(chosen)");
}
