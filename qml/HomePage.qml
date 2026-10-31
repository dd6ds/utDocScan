import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

// Start page: the saved PDFs and the button that starts (or continues) a scan.
// Tapping a document offers open/share, rename and delete.
Page {
    id: page
    property var app

    background: Rectangle { color: page.app.colorBackground }

    header: AppHeader {
        app: page.app
        title: "ubDocScan"
        showBack: false

        Button {
            text: qsTr("Settings")
            onClicked: page.app.openSettings()
        }
    }

    function sizeText(bytes) {
        var locale = Qt.locale(Language.effectiveLanguage)
        if (bytes < 1024 * 1024)
            return qsTr("%1 KB").arg(Math.max(1, Math.round(bytes / 1024)))
        return qsTr("%1 MB").arg(Number(bytes / (1024 * 1024)).toLocaleString(locale, "f", 1))
    }

    function dateText(date) {
        return date.toLocaleString(Qt.locale(Language.effectiveLanguage), Locale.ShortFormat)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: page.app.spacingUnit
        spacing: page.app.spacingUnit

        Button {
            Layout.fillWidth: true
            Layout.preferredHeight: page.app.buttonHeight * 1.3
            highlighted: true
            font.bold: true
            text: Session.pages.length
                  ? qsTr("Continue scan (%n page(s))", "", Session.pages.length)
                  : qsTr("New scan")
            onClicked: page.app.openScan()
        }

        AppLabel {
            text: qsTr("Saved documents")
            font.bold: true
            font.pixelSize: page.app.fontSecondary
            color: page.app.colorSecondaryText
        }

        ListView {
            id: documentList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: Library.documents
            spacing: page.app.gu * 0.5

            delegate: ItemDelegate {
                width: documentList.width
                onClicked: documentMenu.openFor(modelData, this)
                background: Rectangle {
                    color: parent.down ? page.app.colorSurfacePressed : page.app.colorSurface
                    radius: page.app.gu * 0.5
                }
                contentItem: Column {
                    spacing: page.app.gu * 0.3
                    AppLabel {
                        width: parent.width
                        text: modelData.name
                        font.bold: true
                        elide: Text.ElideRight
                        wrapMode: Text.NoWrap
                    }
                    AppLabel {
                        width: parent.width
                        text: page.dateText(modelData.modified) + "  ·  "
                              + page.sizeText(modelData.size)
                        color: page.app.colorSecondaryText
                        font.pixelSize: page.app.fontSecondary
                    }
                }
            }

            AppLabel {
                anchors.centerIn: parent
                width: parent.width * 0.8
                visible: documentList.count === 0
                horizontalAlignment: Text.AlignHCenter
                color: page.app.colorSecondaryText
                text: qsTr("No documents yet. Tap \"New scan\" to scan your first one.")
            }
        }
    }

    DocumentMenu {
        id: documentMenu
        app: page.app
    }
}
