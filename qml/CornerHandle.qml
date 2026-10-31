import QtQuick 2.12

// A draggable corner of the scan frame on CropPage. Position and result are
// normalised to 0..1 of the painted photo; `moved` reports the new point.
Item {
    id: handle
    property var app
    // The Image the corner belongs to (fillMode PreserveAspectFit).
    property Item photo
    property real normX
    property real normY

    signal moved(real nx, real ny)

    readonly property real offsetX: (photo.width - photo.paintedWidth) / 2
    readonly property real offsetY: (photo.height - photo.paintedHeight) / 2

    // Larger than it looks, so it is easy to grab with a finger.
    width: app.gu * 6
    height: width
    x: photo.x + offsetX + normX * photo.paintedWidth - width / 2
    y: photo.y + offsetY + normY * photo.paintedHeight - height / 2

    Rectangle {
        anchors.centerIn: parent
        width: handle.app.gu * 2.6
        height: width
        radius: width / 2
        color: dragArea.pressed ? handle.app.colorScanFrame : "#80000000"
        border.color: handle.app.colorScanFrame
        border.width: handle.app.gu * 0.4
    }

    MouseArea {
        id: dragArea
        anchors.fill: parent
        preventStealing: true
        onPositionChanged: {
            var p = mapToItem(handle.photo, mouse.x, mouse.y)
            var nx = (p.x - handle.offsetX) / handle.photo.paintedWidth
            var ny = (p.y - handle.offsetY) / handle.photo.paintedHeight
            handle.moved(Math.max(0, Math.min(1, nx)), Math.max(0, Math.min(1, ny)))
        }
    }
}
