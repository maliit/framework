import Qt 4.7

Rectangle {
    id: canvas
    width: MInputMethodQuick.screenWidth
    height: MInputMethodQuick.screenHeight

    Rectangle {
        id: imArea
        width: MInputMethodQuick.screenWidth * 0.5
        height: MInputMethodQuick.screenHeight * 0.5
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        transformOrigin: Item.Center
    }

    Component.onCompleted: {
        MInputMethodQuick.setInputMethodArea(Qt.rect(0, MInputMethodQuick.screenHeight - imArea.height,
                                                     imArea.width, imArea.height))
        MInputMethodQuick.sendCommit("Maliit")
        MInputMethodQuick.sendPreedit("Maliit")
    }
}
