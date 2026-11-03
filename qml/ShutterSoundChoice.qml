import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

// Shutter sound on or off. Stored in app.settings.playShutterSound, which the
// camera plugin reads itself (see main.qml).
ColumnLayout {
    id: choice
    property var app

    spacing: 0

    AppLabel {
        text: qsTr("Camera")
        font.bold: true
        font.pixelSize: choice.app.fontSecondary
    }

    RowLayout {
        Layout.fillWidth: true

        AppLabel {
            Layout.fillWidth: true
            text: qsTr("Shutter sound")
        }
        Switch {
            checked: choice.app.settings.playShutterSound
            onToggled: choice.app.settings.playShutterSound = checked
        }
    }
}
