import QtQuick 2.12
import QtQuick.Controls 2.12

// Actions for one saved PDF: open or share it with another app (Content
// Hub), rename it, delete it after a confirmation.
Item {
    id: actions
    property var app
    property var document: ({})

    anchors.fill: parent

    function openFor(doc, item) {
        document = doc
        menu.popup(item, item.width / 2, item.height / 2)
    }

    Menu {
        id: menu

        MenuItem {
            text: qsTr("Open or share...")
            onTriggered: sharePicker.share(actions.document.url)
        }
        MenuItem {
            text: qsTr("Rename")
            onTriggered: {
                nameField.text = actions.document.name
                renameDialog.open()
            }
        }
        MenuItem {
            text: qsTr("Delete")
            onTriggered: deleteDialog.open()
        }
    }

    Dialog {
        id: renameDialog
        anchors.centerIn: parent
        width: parent.width * 0.9
        modal: true
        title: qsTr("Rename document")
        standardButtons: Dialog.Ok | Dialog.Cancel

        AppField {
            id: nameField
            width: parent.width
        }

        onAccepted: {
            if (!Library.rename(actions.document.url, nameField.text))
                actions.app.showNote(qsTr("A document with this name already exists."))
        }
    }

    Dialog {
        id: deleteDialog
        anchors.centerIn: parent
        width: parent.width * 0.9
        modal: true
        title: qsTr("Delete document")
        standardButtons: Dialog.Yes | Dialog.No

        AppLabel {
            width: parent.width
            text: qsTr("Delete \"%1\"? This cannot be undone.").arg(actions.document.name)
        }

        onAccepted: Library.remove(actions.document.url)
    }

    SharePicker {
        id: sharePicker
    }
}
