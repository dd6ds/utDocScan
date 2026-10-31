import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtMultimedia 5.12

// Camera viewfinder. Tap to focus; the shutter stores the photo in the
// session's cache and replaces this page with the crop page for it.
Page {
    id: page
    property var app
    property bool flashOn: false

    background: Rectangle { color: "black" }

    Camera {
        id: camera
        captureMode: Camera.CaptureStillImage
        focus.focusMode: Camera.FocusContinuous
        flash.mode: page.flashOn ? Camera.FlashOn : Camera.FlashOff

        imageCapture.onImageSaved: {
            page.app.stack.replace(Qt.resolvedUrl("CropPage.qml"),
                                   { app: page.app, source: "file://" + path })
        }
        imageCapture.onCaptureFailed: page.app.showNote(qsTr("The photo could not be taken."))
        onError: page.app.showNote(qsTr("The camera is not available: %1").arg(errorString))
    }

    function takePhoto() {
        if (!camera.imageCapture.ready)
            return
        // Tells the camera service how the phone is held, so the photo is
        // stored upright.
        camera.imageCapture.setMetadata("Orientation",
                                        Screen.angleBetween(Screen.orientation,
                                                            Screen.primaryOrientation))
        camera.imageCapture.captureToLocation(Session.newCapturePath())
    }

    VideoOutput {
        id: viewfinder
        anchors.fill: parent
        source: camera
        autoOrientation: true
        fillMode: VideoOutput.PreserveAspectFit

        MouseArea {
            anchors.fill: parent
            onClicked: {
                camera.focus.customFocusPoint = viewfinder.mapPointToSourceNormalized(
                            Qt.point(mouse.x, mouse.y))
                camera.focus.focusPointMode = Camera.FocusPointCustom
                camera.searchAndLock()
            }
        }
    }

    ToolButton {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: page.app.gu
        text: "‹"
        font.pixelSize: page.app.fontTitle * 1.6
        palette.buttonText: "#ffffff"
        Accessible.name: qsTr("Back")
        onClicked: page.app.stack.pop()
    }

    AppLabel {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: page.app.gu * 2
        width: parent.width * 0.6
        horizontalAlignment: Text.AlignHCenter
        color: "#ffffff"
        style: Text.Outline
        styleColor: "#80000000"
        font.pixelSize: page.app.fontSecondary
        text: qsTr("Hold the phone above the page")
    }

    Item {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: page.app.gu * 14

        Button {
            anchors.verticalCenter: shutter.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: page.app.spacingUnit
            text: page.flashOn ? qsTr("Flash on") : qsTr("Flash off")
            onClicked: page.flashOn = !page.flashOn
        }

        RoundButton {
            id: shutter
            anchors.centerIn: parent
            width: page.app.gu * 9
            height: width
            enabled: camera.imageCapture.ready
            Accessible.name: qsTr("Take photo")
            background: Rectangle {
                radius: width / 2
                color: shutter.down ? "#cccccc" : "#ffffff"
                border.color: page.app.colorScanFrame
                border.width: page.app.gu * 0.6
                opacity: shutter.enabled ? 1 : 0.5
            }
            onClicked: page.takePhoto()
        }
    }
}
