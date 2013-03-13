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
    /*
     * Use two properties for each key, one that contains all available labels,
     * one for the index.
     * Bind the key's label to the currently active cycle label (denoted by
     * index) and keep looping through the range of available labels.
     */

    property variant cycle: {}
    property int index: 0
    property string localPreeditBuffer: label.text
    property bool doubleClick: false

    width: 80
    height: 60

    border.width: 2
    border.color: "black"
    color: "blue"

    // The key label, the displayed text is controlled through the cycle index.
    Text {
        id: label
        text: cycle[index]
        color: "white"
        font.pointSize: 20
        anchors.centerIn: parent
    }

    // This timer resets key's background color to default ("blue").
    Timer {
        id: resetTimeout
        interval: 100
        repeat: false

        onTriggered: {
            parent.color = "blue"
        }
    }

    // This timer decides whether to commit current cycle key to application's
    // focus widget.
    Timer {
        id: commitTimeout
        interval: 500
        repeat: false

        onTriggered: {
            doubleClick = false

            if (preeditBuffer.length > 0)
                parent.color = "orange"
                resetTimeout.restart()

                // Once committed, the preedit cannot be edited anymore:
                MInputMethodQuick.sendCommit(preeditBuffer)
                index = 0
                preeditBuffer = ""
        }
    }

    MouseArea {
        anchors.fill: parent

        onPressed: {
            // Give a visual hint that this key is pressed:
            parent.color = "black"
            commitTimeout.stop

            // Commit current preedit buffer, if set by other key:
            if (preeditBuffer.length > 0
                && localPreeditBuffer != preeditBuffer)
                MInputMethodQuick.sendCommit(preeditBuffer)
                localPreeditBuffer = ""
                preeditBuffer = ""

            
            // Change key's label if double click was detected:
            if (doubleClick) {
                index = cycle.length > 0 ? (index + 1) % cycle.length
                                         : 0
            }

            doubleClick = true
        }

        onClicked: {
	    // Change back key background color to default, as key was
	    // released:
            parent.color = "blue"

	    // Store label in (global) preedit buffer, save copy in local
	    // buffer to decide whether preedit buffer was set by this key:
            preeditBuffer = label.text
            localPreeditBuffer = preeditBuffer

	    // Send an updated string to application, called "preedit". It is
	    // not committed yet and therefore can still be edited by the input
	    // method plugin:
            MInputMethodQuick.sendPreedit(preeditBuffer)
            
            commitTimeout.restart()
        }
    }
}
