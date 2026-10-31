import QtQuick 2.12
import QtQuick.Controls 2.12

// Actions for one scanned page. Editing opens the crop page again, which is
// up to ScanPage; the rest goes straight to the session.
Menu {
    id: menu
    property int pageIndex: -1

    signal editRequested(int index)

    function openFor(index, item) {
        pageIndex = index
        popup(item, item.width / 2, item.height / 2)
    }

    MenuItem {
        text: qsTr("Edit corners and filter")
        onTriggered: menu.editRequested(menu.pageIndex)
    }
    MenuItem {
        text: qsTr("Rotate")
        onTriggered: Session.rotatePage(menu.pageIndex)
    }
    MenuItem {
        text: qsTr("Move earlier")
        enabled: menu.pageIndex > 0
        onTriggered: Session.movePage(menu.pageIndex, menu.pageIndex - 1)
    }
    MenuItem {
        text: qsTr("Move later")
        enabled: menu.pageIndex < Session.pages.length - 1
        onTriggered: Session.movePage(menu.pageIndex, menu.pageIndex + 1)
    }
    MenuItem {
        text: qsTr("Delete page")
        onTriggered: Session.removePage(menu.pageIndex)
    }
}
