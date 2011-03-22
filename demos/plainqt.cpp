/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtCore>
#include <QtGui>
#include <QX11Info>
#include <stdlib.h>

namespace {
    const char * const TextPrompt = "Double-click this text area to invoke virtual keyboard ...";
    const char * const ImModuleEnv = "QT_IM_MODULE";
    const char * const ExpectedImModule = "MInputContext";


    QString checkForErrors()
    {
        const char * const imModule = getenv(ImModuleEnv);
        QString errorMsg;

        if (not imModule 
            || (0 != strcmp(getenv(ImModuleEnv), ExpectedImModule))) {
            errorMsg.append(QString("You will not be able to use the virtual keyboard. The " \
                                    "environment variable %1 must be set to %2.")
                            .arg(ImModuleEnv)
                            .arg(ExpectedImModule));
        }

        if (not QX11Info::isCompositingManagerRunning()) {
            if (not errorMsg.isEmpty()) {
                errorMsg.append("\n");
            }

            errorMsg.append("No compositing window manager found. You might be able to run the " \
                            "MeeGo Input Method server in self-compositing mode:\n" \
                            "$ meego-im-uiserver -use-self-composition -manual-redirection");
        }

        return errorMsg;
    }
}

class MyTextEdit
    : public QTextEdit
{
private:
    bool wasFocused;

public:
    MyTextEdit()
        : QTextEdit(TextPrompt)
        , wasFocused(false)
    {}

protected:
    void focusInEvent(QFocusEvent *e)
    {
        toPlainText();
        // On first text edit, clear pre-set TextPrompt:
        if (not wasFocused && toPlainText() == QString(TextPrompt)) {
            wasFocused = true;
            setText("");
        }

        QTextEdit::focusInEvent(e);
    }
};

int main(int argc, char** argv)
{
    QApplication kit(argc, argv);
    QMainWindow w;
    w.setWindowTitle("Maliit test application");

    QVBoxLayout *vbox = new QVBoxLayout;

    // Clicking the button will steal focus from the text edit, thus hiding
    // the virtual keyboard:
    QPushButton *hideVkb = new QPushButton("Hide virtual keyboard");
    vbox->addWidget(hideVkb);

    QTextEdit *e = new MyTextEdit;
    vbox->addWidget(e);

    QPushButton *closeApp = new QPushButton("Close application");
    vbox->addWidget(closeApp);
    QObject::connect(closeApp, SIGNAL(clicked()),
                     &kit,     SLOT(quit()));

    w.setCentralWidget(new QWidget);
    w.centralWidget()->setLayout(vbox);
    w.show();

    const QString &errorMsg(checkForErrors());

    if (not errorMsg.isEmpty()) {
        e->setText(errorMsg);
        qCritical() << "CRITICAL:" << errorMsg;
    }

    return kit.exec();
}
