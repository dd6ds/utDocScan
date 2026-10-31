import QtQuick 2.12
import QtQuick.Controls 2.12

// Covers the window while ScanSession works, so nothing can be tapped
// meanwhile. While a PDF is written it shows which page is being done.
Rectangle {
    id: overlay
    property var app

    readonly property int pageCount: Session.pages.length
    readonly property int currentPage: Math.min(Session.exportedPages + 1, pageCount)

    color: app.darkMode ? "#cc000000" : "#ccffffff"

    // Swallows taps on the pages below.
    MouseArea {
        anchors.fill: parent
    }

    Column {
        anchors.centerIn: parent
        width: parent.width - 4 * app.spacingUnit
        spacing: app.spacingUnit

        BusyIndicator {
            anchors.horizontalCenter: parent.horizontalCenter
            running: overlay.visible
        }

        AppLabel {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            text: overlay.app.exportMode === "ocr"
                  ? qsTr("Recognising text: page %1 of %2").arg(overlay.currentPage)
                                                           .arg(overlay.pageCount)
                  : overlay.app.exportMode === "images"
                    ? qsTr("Writing PDF: page %1 of %2").arg(overlay.currentPage)
                                                       .arg(overlay.pageCount)
                    : qsTr("Processing page...")
        }

        AppLabel {
            width: parent.width
            visible: overlay.app.exportMode === "ocr"
            horizontalAlignment: Text.AlignHCenter
            color: overlay.app.colorSecondaryText
            font.pixelSize: overlay.app.fontSecondary
            text: qsTr("This takes a few seconds per page.")
        }
    }
}
