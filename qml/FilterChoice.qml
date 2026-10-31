import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

// Colour, grey or black-and-white for a page. The numbers match
// PageProcessor::Filter in C++.
RowLayout {
    id: choice
    property var app
    property int filter

    signal selected(int filter)

    spacing: app.gu * 0.5

    Repeater {
        model: [
            { filter: 0, label: qsTr("Colour") },
            { filter: 1, label: qsTr("Grey") },
            { filter: 2, label: qsTr("Black & white") }
        ]
        Button {
            Layout.fillWidth: true
            Layout.preferredHeight: choice.app.buttonHeight * 0.85
            checkable: true
            checked: choice.filter === modelData.filter
            text: modelData.label
            font.pixelSize: choice.app.fontSecondary
            onClicked: choice.selected(modelData.filter)
        }
    }
}
