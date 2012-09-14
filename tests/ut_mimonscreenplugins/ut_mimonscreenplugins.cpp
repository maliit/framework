/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "ut_mimonscreenplugins.h"

#include "mimonscreenplugins.h"
#include "mimsettings.h"
#include "mimsettingsqsettings.h"

#include <QtCore>

namespace
{
    const QString ORGANIZATION = "maliit.org";
    const QString APPLICATION = "server-tests";
}

void Ut_MImOnScreenPlugins::initTestCase()
{
    MImSettings::setImplementationFactory(new MImSettingsQSettingsBackendFactory);
}

void Ut_MImOnScreenPlugins::cleanupTestCase()
{}

void Ut_MImOnScreenPlugins::init()
{
    QSettings settings(ORGANIZATION, APPLICATION);
    settings.clear();
}

void Ut_MImOnScreenPlugins::cleanup()
{}

QTEST_MAIN(Ut_MImOnScreenPlugins)
