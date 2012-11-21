/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 One Laptop per Child Association
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef UT_MIMPLUGINMANAGERCONFIG_H
#define UT_MIMPLUGINMANAGERCONFIG_H

#include "mimserveroptions.h"
#include "mimsettingsqsettings.h"
#include <tr1/memory>

#include <QtTest/QtTest>
#include <QObject>

class MIMPluginManager;
class MIMPluginManagerPrivate;
class MInputContextConnection;
class QDBusInterface;

class Ut_MIMPluginManagerConfig : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testNoActiveSubView();
    void testEmptyConfig();
    void activeButNoEnabledSubview();

private:
    QString pluginPath;
    MImSettings *enabledPluginSettings;
    MImSettings *activePluginSettings;
    MIMPluginManager *manager;
    MIMPluginManagerPrivate *subject;
    MInputContextConnection *connection;
};

#endif
