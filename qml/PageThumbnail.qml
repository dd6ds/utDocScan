import QtQuick 2.12
import QtQuick.Controls 2.12

// One scanned page in the grid of ScanPage, with its page number.
ItemDelegate {
    id: thumbnail
    property var app
    property url source
    property int number

    padding: app.gu * 0.5

    background: Rectangle {
        color: thumbnail.down ? thumbnail.app.colorSurfacePressed : "transparent"
        radius: thumbnail.app.gu * 0.5
    }

    contentItem: Item {
        Rectangle {
            anchors.fill: image
            anchors.margins: -1
            color: "transparent"
            border.color: thumbnail.app.colorBorder
        }

        Image {
            id: image
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            asynchronous: true
            // Thumbnails only; the full page is ~3500 px high.
            sourceSize.height: Math.round(parent.height * 1.5)
            source: thumbnail.source
        }

        Rectangle {
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: thumbnail.app.gu * 0.5
            width: Math.max(height, numberLabel.implicitWidth + thumbnail.app.gu)
            height: numberLabel.implicitHeight + thumbnail.app.gu * 0.4
            radius: height / 2
            color: thumbnail.app.colorAccent

            Text {
                id: numberLabel
                anchors.centerIn: parent
                text: thumbnail.number
                color: "#ffffff"
                font.bold: true
                font.pixelSize: thumbnail.app.fontSecondary
            }
        }
    }
}
