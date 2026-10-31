import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

// The document being scanned: its pages as thumbnails, adding pages from the
// camera or another app, editing, rotating, reordering and deleting them,
// and saving the result as PDF. Going back keeps the pages for later.
Page {
    id: page
    property var app
    // Opens the camera once when an empty document is started.
    property bool cameraOffered: false

    background: Rectangle { color: page.app.colorBackground }

    header: AppHeader {
        app: page.app
        title: qsTr("New document")
        onBack: page.app.stack.pop()

        Button {
            visible: Session.pages.length > 0
            text: qsTr("Discard")
            onClicked: discardDialog.open()
        }
    }

    function openCamera() {
        app.stack.push(Qt.resolvedUrl("CameraPage.qml"), { app: app })
    }

    function openCrop(properties) {
        properties.app = app
        app.stack.push(Qt.resolvedUrl("CropPage.qml"), properties)
    }

    function editPage(index) {
        var settings = Session.pageSettings(index)
        openCrop({ source: settings.source, corners: settings.corners,
                   filter: settings.filter, pageIndex: index })
    }

    StackView.onActivated: {
        if (!cameraOffered && Session.pages.length === 0) {
            cameraOffered = true
            openCamera()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: page.app.spacingUnit
        spacing: page.app.spacingUnit

        GridView {
            id: pageGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: Session.pages
            readonly property int columns: width > height ? 4 : 2
            cellWidth: width / columns
            cellHeight: cellWidth * 1.414

            delegate: PageThumbnail {
                app: page.app
                width: pageGrid.cellWidth
                height: pageGrid.cellHeight
                source: modelData
                number: index + 1
                onClicked: pageMenu.openFor(index, this)
            }

            AppLabel {
                anchors.centerIn: parent
                width: parent.width * 0.8
                visible: pageGrid.count === 0
                horizontalAlignment: Text.AlignHCenter
                color: page.app.colorSecondaryText
                text: qsTr("Add pages with the camera or import a photo.")
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: page.app.gu

            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: page.app.buttonHeight
                text: qsTr("Camera")
                onClicked: page.openCamera()
            }
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: page.app.buttonHeight
                text: qsTr("Import")
                onClicked: importPicker.open()
            }
        }

        Button {
            Layout.fillWidth: true
            Layout.preferredHeight: page.app.buttonHeight
            highlighted: true
            font.bold: true
            enabled: Session.pages.length > 0
            text: qsTr("Save as PDF")
            onClicked: page.app.stack.push(Qt.resolvedUrl("ExportPage.qml"), { app: page.app })
        }
    }

    PageMenu {
        id: pageMenu
        onEditRequested: page.editPage(index)
    }

    ImportPicker {
        id: importPicker
        onImported: page.openCrop({ source: url })
    }

    Dialog {
        id: discardDialog
        anchors.centerIn: parent
        width: parent.width * 0.9
        modal: true
        title: qsTr("Discard document")
        standardButtons: Dialog.Yes | Dialog.No

        AppLabel {
            width: parent.width
            text: qsTr("Delete all scanned pages of this document?")
        }

        onAccepted: {
            Session.clear()
            page.app.stack.pop()
        }
    }
}
