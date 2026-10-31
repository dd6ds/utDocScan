// Entry point: sets up logging, the language, the QML engine, and exposes the
// backend (ScanSession, DocumentLibrary, LanguageManager) and the phone's
// light/dark theme (SystemTheme) to QML.

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QTextStream>

#include <clocale>

#include "documentlibrary.h"
#include "languagemanager.h"
#include "scansession.h"
#include "systemtheme.h"

namespace {

// The click package name. Confined apps may only write below
// ~/.local/share, ~/.cache and ~/.config /<package name>.
const char *const kPackageName = "ubdocscan";

QString logFilePath()
{
    static const QString path = [] {
        const QString dir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
                            + QLatin1Char('/') + QLatin1String(kPackageName);
        QDir().mkpath(dir);
        return dir + QStringLiteral("/ubdocscan.log");
    }();
    return path;
}

QtMessageHandler defaultHandler = nullptr;

// Writes Qt's messages (qDebug, QML warnings) to the app log as well as to
// the journal (journalctl --user -f on the phone).
void logHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    QFile logFile(logFilePath());
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream(&logFile) << QDateTime::currentDateTime().toString(
                                     QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"))
                              << ' ' << message << '\n';
    }
    if (defaultHandler)
        defaultHandler(type, context, message);
}

// Ubuntu Touch exports GRID_UNIT_PX (typically 18-27 on a phone, 8 on a
// desktop). All QML sizes are multiples of it, so text stays readable on a
// high-DPI phone screen.
int gridUnitPx()
{
    bool ok = false;
    const int px = qEnvironmentVariableIntValue("GRID_UNIT_PX", &ok);
    return (ok && px > 0) ? px : 8;
}

void exposeBackend(QQmlApplicationEngine &engine)
{
    // Owned by the engine, so they outlive every QML binding at shutdown.
    QQmlContext *context = engine.rootContext();
    context->setContextProperty(QStringLiteral("Language"), new LanguageManager(&engine, &engine));
    context->setContextProperty(QStringLiteral("Session"), new ScanSession(&engine));
    context->setContextProperty(QStringLiteral("Library"), new DocumentLibrary(&engine));
    context->setContextProperty(QStringLiteral("SystemTheme"), new SystemTheme(&engine));
    context->setContextProperty(QStringLiteral("gridUnitPx"), gridUnitPx());
}

} // namespace

int main(int argc, char *argv[])
{
    // The Default Controls 2 style; the platform style brings its own palette,
    // and controls without explicit colours then render dark on dark.
    qputenv("QT_QUICK_CONTROLS_STYLE", "Default");

    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("ubDocScan"));
    // Also decides the settings path (~/.config/ubdocscan/ubDocScan.conf)
    // and the data and cache folders the backend uses.
    app.setOrganizationName(QLatin1String(kPackageName));
    defaultHandler = qInstallMessageHandler(logHandler);
    qDebug() << "=== ubDocScan starting ===";

    // Qt switched the C library to the phone's locale; Tesseract parses its
    // model files with the C library and needs "." as decimal separator.
    std::setlocale(LC_NUMERIC, "C");

    QQmlApplicationEngine engine;
    exposeBackend(engine);
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        qWarning() << "Failed to load QML - no root objects";
        return -1;
    }
    return app.exec();
}
