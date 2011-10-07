/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

import Qt 4.7

Rectangle {
    id: canvas
    width: MInputMethodQuick.screenWidth
    height: MInputMethodQuick.screenHeight
    color: "transparent"

    Rectangle {
        id: imArea
        width: MInputMethodQuick.screenWidth
        height: MInputMethodQuick.screenHeight * 0.5
        radius: 10
        color: "grey"
        border.color: "black"
        border.width: 10
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        Text {
            id: buttonText
            text: MInputMethodQuick.actionKeyOverride.label
            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent
            onClicked: MInputMethodQuick.activateActionKey()
        }
    }

    Component.onCompleted: {
        MInputMethodQuick.setInputMethodArea(Qt.rect(0, MInputMethodQuick.screenHeight - imArea.height,
                                                     imArea.width, imArea.height))
        MInputMethodQuick.actionKeyOverride.setLabel("Enter")
        MInputMethodQuick.sendCommit("Maliit")
        MInputMethodQuick.sendPreedit("Maliit")
    }
}
