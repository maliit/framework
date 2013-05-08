import QtQuick 2.0

Rectangle {
    width: 854
    height: 486
    color: "grey"

    TextInput {
        width: parent.width
        height: 50
        font.pixelSize: 50
        focus: true
        property variant __inputMethodExtensions: {
            'enterKeyText': 'Enter',
            'enterKeyHighlighted': true
        }
    }
}
