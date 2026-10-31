import QtQuick 2.12
import QtQuick.Controls 2.12
import Lomiri.Content 1.3

// Hands a saved PDF to another app through the Content Hub: a document
// viewer to open it, the file manager to store it elsewhere, or a messenger.
Popup {
    id: picker
    property string fileUrl

    function share(url) {
        fileUrl = url
        open()
    }

    parent: Overlay.overlay
    width: parent ? parent.width : 0
    height: parent ? parent.height : 0
    padding: 0
    modal: true

    // Created on first use: it pulls in the Lomiri toolkit, which would
    // otherwise slow down the app start. It stays afterwards, so closing the
    // popup from its own signal handlers does not destroy it mid-handler.
    property bool used: false
    onOpened: used = true

    Loader {
        anchors.fill: parent
        active: picker.used
        sourceComponent: Component {
            ContentPeerPicker {
                handler: ContentHandler.Destination
                contentType: ContentType.Documents
                headerText: qsTr("Open or share with")

                onPeerSelected: {
                    var transfer = peer.request()
                    transfer.items = [ itemComponent.createObject(picker, { url: picker.fileUrl }) ]
                    transfer.state = ContentTransfer.Charged
                    picker.close()
                }
                onCancelPressed: picker.close()
            }
        }
    }

    Component {
        id: itemComponent
        ContentItem {}
    }
}
