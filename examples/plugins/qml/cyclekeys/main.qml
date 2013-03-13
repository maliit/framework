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

import QtQuick 2.0

Rectangle {
    id: canvas
    width: MInputMethodQuick.screenWidth
    height: MInputMethodQuick.screenHeight
    color: "transparent"

    property string preeditBuffer

    Rectangle {
        id: imArea
        color: "grey"
        width: MInputMethodQuick.screenWidth
        height: MInputMethodQuick.screenHeight * 0.5
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        Row {
            spacing: 6
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            CycleKey {
                id: key0
                cycle: ["A", "b", "C"]
            }

            CycleKey {
                id: key1
                cycle: ["1", "2", "3"]
            }

            CycleKey {
                id: key2
                cycle: ["#", "?", "!"]
            }
        }
    }

    Component.onCompleted: {
        MInputMethodQuick.setInputMethodArea(Qt.rect(0, MInputMethodQuick.screenHeight - imArea.height,
                                                     imArea.width, imArea.height))

        /* Ignore this part, only used for sanity check when running tests. */
        MInputMethodQuick.sendCommit("Maliit")
        MInputMethodQuick.sendPreedit("Maliit")
    }
}
