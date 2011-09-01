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

#include "utils.h"

#include <QtDebug>
#include <QtCore>
#include <QtGui>

namespace {
    const QString TestingInSandboxEnvVariable("TESTING_IN_SANDBOX");

    const QString GlobalTestPluginPath(MALIIT_TEST_PLUGINS_DIR);
    const QString TestPluginPathEnvVariable("TESTPLUGIN_PATH");
}

namespace MaliitTestUtils {

/* Return true if we are testing against the repository tree,
or false if testing against installed software. */
bool isTestingInSandbox()
{
    bool testingInSandbox = false;
    const QStringList env(QProcess::systemEnvironment());
    int index = env.indexOf(QRegExp('^' + TestingInSandboxEnvVariable + "=.*", Qt::CaseInsensitive));
    if (index != -1) {
        QString statusCandidate = env.at(index);
        statusCandidate = statusCandidate.remove(
                              QRegExp('^' + TestingInSandboxEnvVariable + '=', Qt::CaseInsensitive));
        bool statusOk = false;
        int status = statusCandidate.toInt(&statusOk);
        if (statusOk && (status == 0 || status == 1)) {
            testingInSandbox = (status == 1);
        } else {
            qCritical() << "Invalid " << TestingInSandboxEnvVariable << " environment variable.\n";
        }
    }
    return testingInSandbox;
}

// Use either global test plugin directory or TESTPLUGIN_PATH, if it is
// set (to local sandbox's plugin directory by makefile, at least).
//
// Returns a null QString on failure
QString getTestPluginPath()
{
    QString pluginPath = GlobalTestPluginPath;

    const QStringList env(QProcess::systemEnvironment());
    int index = env.indexOf(QRegExp('^' + TestPluginPathEnvVariable + "=.*", Qt::CaseInsensitive));
    if (index != -1) {
        QString pathCandidate = env.at(index);
        pathCandidate = pathCandidate.remove(
                            QRegExp('^' + TestPluginPathEnvVariable + '=', Qt::CaseInsensitive));
        if (!pathCandidate.isEmpty()) {
            pluginPath = pathCandidate;
        } else {
            qCritical() << "Invalid " << TestPluginPathEnvVariable << " environment variable.\n";
            return QString();
        }
    }
    if (!QDir(pluginPath).exists()) {
        qCritical("Test plugin directory does not exist.");
        return QString();
    }
    return pluginPath;
}

// Wait for signal or timeout; use SIGNAL macro for signal
void waitForSignal(const QObject* object, const char* signal, int timeout)
{
    QEventLoop eventLoop;
    QObject::connect(object, signal, &eventLoop, SLOT(quit()));
    QTimer::singleShot(timeout, &eventLoop, SLOT(quit()));
    eventLoop.exec();
}

RemoteWindow::RemoteWindow(QWidget *p, Qt::WindowFlags f)
    : QWidget(p, f)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
}

void RemoteWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setBrush(QBrush(QColor(Qt::green)));
    p.drawRect(QRect(QPoint(), size()));
    QFont f;
    f.setPointSize(32);
    p.setFont(f);
    p.drawText(QRect(QPoint(), size()).adjusted(16, 16, -16, -16),
               QString("Maliit"));
}

}
