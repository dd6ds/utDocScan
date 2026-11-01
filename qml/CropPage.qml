import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

// Places the scan frame on a photo (four draggable corners) and picks the
// filter; "Use page" hands both to the session, which straightens the page.
// Also used to edit a page again (pageIndex >= 0).
Page {
    id: page
    property var app
    property string source
    // x0, y0 ... x3, y3, clockwise from top left, normalised to 0..1.
    property var corners: insetCorners
    property int filter: app.settings.filter
    property int pageIndex: -1

    readonly property var insetCorners: [0.05, 0.05, 0.95, 0.05, 0.95, 0.95, 0.05, 0.95]
    readonly property var fullCorners: [0, 0, 1, 0, 1, 1, 0, 1]

    background: Rectangle { color: page.app.colorBackground }

    header: AppHeader {
        app: page.app
        title: page.pageIndex < 0 ? qsTr("Adjust page") : qsTr("Edit page %1").arg(page.pageIndex + 1)
        onBack: page.app.stack.pop()
    }

    function setCorner(i, x, y) {
        var c = corners.slice()
        c[2 * i] = x
        c[2 * i + 1] = y
        corners = c
    }

    function accept() {
        app.settings.filter = filter
        Session.processPage(source, corners, filter, pageIndex)
        app.stack.pop()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: page.app.spacingUnit
        spacing: page.app.spacingUnit

        Item {
            id: photoArea
            Layout.fillWidth: true
            Layout.fillHeight: true

            Image {
                id: photoImage
                anchors.fill: parent
                anchors.margins: page.app.gu * 1.5
                fillMode: Image.PreserveAspectFit
                autoTransform: true
                asynchronous: true
                // Enough for placing corners; the full photo is processed in C++.
                sourceSize.height: 2048
                source: page.source
            }

            ScanFrame {
                anchors.fill: parent
                app: page.app
                photo: photoImage
                corners: page.corners
                visible: photoImage.status === Image.Ready
            }

            Repeater {
                model: photoImage.status === Image.Ready ? 4 : 0
                CornerHandle {
                    app: page.app
                    photo: photoImage
                    normX: page.corners[2 * index]
                    normY: page.corners[2 * index + 1]
                    onMoved: page.setCorner(index, nx, ny)
                }
            }

            BusyIndicator {
                anchors.centerIn: parent
                running: photoImage.status === Image.Loading
            }
        }

        FilterChoice {
            Layout.fillWidth: true
            app: page.app
            filter: page.filter
            onSelected: page.filter = filter
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: page.app.gu

            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: page.app.buttonHeight
                text: qsTr("Whole photo")
                onClicked: page.corners = page.fullCorners
            }
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: page.app.buttonHeight
                highlighted: true
                font.bold: true
                enabled: photoImage.status === Image.Ready
                text: qsTr("Use page")
                onClicked: page.accept()
            }
        }
    }
}
