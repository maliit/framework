/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "core-utils.h"

#include <QtDebug>
#include <QtCore>
#include <QtTest>

namespace {
    const QString TestingInSandboxEnvVariable("TESTING_IN_SANDBOX");

    const QString GlobalTestPluginPath(MALIIT_TEST_PLUGINS_DIR);
    const QString TestPluginPathEnvVariable("TESTPLUGIN_PATH");

    const QString GlobalTestDataPath(MALIIT_TEST_DATA_PATH);
    const QString TestDataPathEnvVariable("TESTDATA_PATH");

    // If the environment variable envVar has a value, set *path to this value
    // Returns true on success, false on error
    bool setPathFromEnvironmentVariable(QString *path, QString envVar) {
        const QStringList env(QProcess::systemEnvironment());
        int index = env.indexOf(QRegExp('^' + envVar + "=.*", Qt::CaseInsensitive));

        if (index != -1) {
            QString pathCandidate = env.at(index);
            pathCandidate = pathCandidate.remove(
                                QRegExp('^' + envVar + '=', Qt::CaseInsensitive));
            if (!pathCandidate.isEmpty()) {
                *path = pathCandidate;
                return true;
            } else {
                qCritical() << "Invalid " << envVar << " environment variable.\n";
                return false;
            }
        }
        return true;

    }
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
    bool success = setPathFromEnvironmentVariable(&pluginPath, TestPluginPathEnvVariable);

    if (!success) {
        return QString();
    }

    if (!QDir("./").exists(pluginPath)) {
        qCritical("Test plugin directory does not exist.");
        return QString();
    }
    return pluginPath;
}

// Use either global test plugin directory or TESTDATA_PATH, if it is
// set (to local sandbox's plugin directory by makefile, at least).
//
// The test data path is the base directory where the tests directories (like ut_mtoolbardata) reside.
//
// Returns a null QString on failure
QString getTestDataPath()
{
    QString dataPath = GlobalTestDataPath;
    bool success = setPathFromEnvironmentVariable(&dataPath, TestDataPathEnvVariable);

    if (!success) {
        return QString();
    }

    if (!QDir("./").exists(dataPath)) {
        qCritical("Test data directory does not exist.");
        return QString();
    }
    return dataPath;
}


// Wait for signal or timeout; use SIGNAL macro for signal
void waitForSignal(const QObject* object, const char* signal, int timeout)
{
    QEventLoop eventLoop;
    QObject::connect(object, signal, &eventLoop, SLOT(quit()));
    QTimer::singleShot(timeout, &eventLoop, SLOT(quit()));
    eventLoop.exec();
}

}
