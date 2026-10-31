import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

// Settings: user interface language and light/dark appearance. Both apply at
// once and are remembered.
Page {
    id: page
    property var app

    background: Rectangle { color: page.app.colorBackground }

    header: AppHeader {
        app: page.app
        title: qsTr("Settings")
        onBack: page.app.stack.pop()
    }

    Flickable {
        anchors.fill: parent
        contentHeight: content.implicitHeight + 2 * page.app.spacingUnit
        clip: true

        ColumnLayout {
            id: content
            x: page.app.spacingUnit
            y: page.app.spacingUnit
            width: parent.width - 2 * page.app.spacingUnit
            spacing: page.app.spacingUnit * 1.5

            LanguageChoice {
                Layout.fillWidth: true
                app: page.app
            }

            AppearanceChoice {
                Layout.fillWidth: true
                app: page.app
            }

            AppLabel {
                Layout.fillWidth: true
                color: page.app.colorSecondaryText
                font.pixelSize: page.app.fontSecondary
                text: qsTr("Text recognition: Tesseract OCR. The languages for it are chosen when saving a PDF.")
            }
        }
    }
}
