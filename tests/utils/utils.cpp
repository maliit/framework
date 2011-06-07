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

namespace {
    const QString TestingInSandboxEnvVariable("TESTING_IN_SANDBOX");
}

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
