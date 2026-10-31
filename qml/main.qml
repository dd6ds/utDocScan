// Ubuntu Touch 20.04 ships Qt 5.12 - importing 2.15 makes the whole file fail
// to load. 2.12 is the highest version actually available on the device.
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import Qt.labs.settings 1.0

// Application window: scaling, colours, settings, the page stack and the
// overlays every page shares (busy indicator, short notes). The pages live in
// their own files and get this window as `app`.
ApplicationWindow {
    id: rootWindow
    visible: true
    width: Screen.width
    height: Screen.height
    title: "ubDocScan"
    color: colorBackground

    // --- Scaling -----------------------------------------------------------
    // gridUnitPx comes from main.cpp (Ubuntu Touch's GRID_UNIT_PX).
    readonly property real gu: gridUnitPx
    readonly property int fontTitle:     Math.round(gu * 2.0)
    readonly property int fontBody:      Math.round(gu * 1.6)
    readonly property int fontSecondary: Math.round(gu * 1.3)
    readonly property int spacingUnit:   Math.round(gu * 1.5)
    readonly property int buttonHeight:  Math.round(gu * 4.5)

    font.pixelSize: fontBody

    // --- Colours -----------------------------------------------------------
    // settings.themeMode: "system" follows the phone's Lomiri theme, "light"
    // and "dark" are fixed. Every colour in the app comes from here; the
    // palette hands them to the stock controls.
    readonly property bool darkMode: settings.themeMode === "dark"
                                     || (settings.themeMode === "system" && SystemTheme.dark)
    readonly property color colorBackground:    darkMode ? "#121212" : "#ffffff"
    readonly property color colorSurface:       darkMode ? "#1f2326" : "#f0f0f0"
    readonly property color colorSurfacePressed: darkMode ? "#2d4a66" : "#b0d4ff"
    readonly property color colorFieldBackground: darkMode ? "#262a2d" : "#ffffff"
    readonly property color colorText:          darkMode ? "#eeeeee" : "#111111"
    readonly property color colorSecondaryText: darkMode ? "#a8a8a8" : "#666666"
    readonly property color colorPlaceholder:   darkMode ? "#777777" : "#999999"
    readonly property color colorBorder:        darkMode ? "#4a4f53" : "#cccccc"
    readonly property color colorAccent:        darkMode ? "#4aa3f0" : "#0d47a1"
    readonly property color colorError:         darkMode ? "#ef5350" : "#c62828"
    // The scan frame on the photo; must stand out on any paper and table.
    readonly property color colorScanFrame:     "#00e5ff"

    palette.window: colorBackground
    palette.windowText: colorText
    palette.base: colorFieldBackground
    palette.alternateBase: colorSurface
    palette.text: colorText
    palette.button: colorSurface
    palette.buttonText: colorText
    palette.brightText: "#ffffff"
    palette.highlight: colorAccent
    palette.highlightedText: "#ffffff"
    palette.light: darkMode ? "#2c3134" : "#f5f5f5"
    palette.midlight: darkMode ? "#33383b" : "#e0e0e0"
    palette.mid: colorBorder
    palette.dark: colorAccent
    palette.shadow: "#000000"

    Settings {
        id: settings
        property string themeMode: "system"
        // Tesseract codes joined with "+"; "" until the user picks some.
        property string ocrLanguages: ""
        property bool ocrEnabled: true
        property int filter: 0
    }
    property alias settings: settings

    // Reading effectiveLanguage makes the default follow a language change.
    readonly property string ocrLanguages: settings.ocrLanguages.length
                                           ? settings.ocrLanguages
                                           : Language.effectiveLanguage
                                             && Language.defaultOcrLanguages()

    // While a PDF is written: "ocr" or "images", for the busy overlay's text.
    property string exportMode: ""

    // --- Navigation --------------------------------------------------------
    function openScan() {
        stack.push(Qt.resolvedUrl("ScanPage.qml"), { app: rootWindow })
    }

    function openSettings() {
        stack.push(Qt.resolvedUrl("SettingsPage.qml"), { app: rootWindow })
    }

    // --- Notes -------------------------------------------------------------
    function showNote(text) {
        note.show(text)
    }

    Connections {
        target: Session
        onFailed: rootWindow.showNote(message)
    }

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: HomePage { app: rootWindow }
    }
    property alias stack: stack

    BusyOverlay {
        app: rootWindow
        anchors.fill: parent
        visible: Session.busy
    }

    Note {
        id: note
        app: rootWindow
    }
}
