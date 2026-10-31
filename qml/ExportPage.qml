import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

// Saves the scanned pages as PDF: title, whether to recognise the text (a
// searchable PDF) and in which languages. Afterwards the scan is cleared and
// the start page shows the new document.
Page {
    id: page
    property var app

    background: Rectangle { color: page.app.colorBackground }

    header: AppHeader {
        app: page.app
        title: qsTr("Save as PDF")
        onBack: page.app.stack.pop()
    }

    function save() {
        var ocr = ocrSwitch.checked
        app.settings.ocrEnabled = ocr
        app.settings.ocrLanguages = languageChoice.selection
        app.exportMode = ocr ? "ocr" : "images"
        Session.exportPdf(titleField.text, ocr ? languageChoice.selection : "")
    }

    Connections {
        target: Session
        onExportFinished: {
            page.app.exportMode = ""
            Session.clear()
            Library.refresh()
            // The file name can differ from the title ("Scan (2)", no slashes).
            var name = decodeURIComponent(url.substring(url.lastIndexOf("/") + 1))
            page.app.showNote(qsTr("Saved \"%1\"").arg(name.replace(/\.pdf$/, "")))
            page.app.stack.pop(null)
        }
        onFailed: page.app.exportMode = ""
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
            spacing: page.app.spacingUnit

            AppLabel {
                text: qsTr("Title")
                font.bold: true
            }
            AppField {
                id: titleField
                Layout.fillWidth: true
                text: qsTr("Scan %1").arg(Qt.formatDateTime(new Date(), "yyyy-MM-dd hh.mm"))
                inputMethodHints: Qt.ImhNoPredictiveText
            }

            AppLabel {
                Layout.fillWidth: true
                color: page.app.colorSecondaryText
                font.pixelSize: page.app.fontSecondary
                text: qsTr("%n page(s)", "", Session.pages.length)
            }

            RowLayout {
                Layout.fillWidth: true
                AppLabel {
                    Layout.fillWidth: true
                    text: qsTr("Recognise text (OCR)")
                    font.bold: true
                }
                Switch {
                    id: ocrSwitch
                    checked: page.app.settings.ocrEnabled
                }
            }

            AppLabel {
                Layout.fillWidth: true
                color: page.app.colorSecondaryText
                font.pixelSize: page.app.fontSecondary
                text: ocrSwitch.checked
                      ? qsTr("The PDF gets an invisible text layer: you can search it and copy text from it.")
                      : qsTr("The PDF contains the page images only.")
            }

            AppLabel {
                visible: ocrSwitch.checked
                text: qsTr("Languages in the document")
                font.bold: true
            }
            OcrLanguageChoice {
                id: languageChoice
                Layout.fillWidth: true
                visible: ocrSwitch.checked
                app: page.app
                selection: page.app.ocrLanguages
            }

            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: page.app.buttonHeight
                Layout.topMargin: page.app.spacingUnit
                highlighted: true
                font.bold: true
                enabled: titleField.text.trim().length > 0
                text: qsTr("Save")
                onClicked: page.save()
            }
        }
    }
}
