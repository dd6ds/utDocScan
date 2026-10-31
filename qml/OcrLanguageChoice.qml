import QtQuick 2.12
import QtQuick.Controls 2.12

// Languages the text recognition looks for, as Tesseract codes joined with
// "+" ("deu+eng"). At least one stays selected.
Flow {
    id: choice
    property var app
    property string selection

    spacing: app.gu

    function toggle(code, on) {
        var codes = selection.length ? selection.split("+") : []
        var at = codes.indexOf(code)
        if (on && at < 0)
            codes.push(code)
        else if (!on && at >= 0 && codes.length > 1)
            codes.splice(at, 1)
        selection = codes.join("+")
    }

    Repeater {
        model: Language.ocrLanguages
        CheckBox {
            id: box
            text: modelData.name
            checked: choice.selection.split("+").indexOf(modelData.code) >= 0
            onToggled: {
                choice.toggle(modelData.code, checked)
                // Re-binds checked in case the last language was kept.
                checked = Qt.binding(function() {
                    return choice.selection.split("+").indexOf(modelData.code) >= 0
                })
            }
            contentItem: AppLabel {
                leftPadding: box.indicator.width + box.spacing
                text: box.text
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
