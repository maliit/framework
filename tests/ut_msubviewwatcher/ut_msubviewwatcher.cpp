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

#include "ut_msubviewwatcher.h"
#include <msubviewwatcher.h>
#include <minputcontextconnection.h>
#include <mimonscreenplugins.h>
#include "mimsettings_stub.h"
#include "fakegconf.h"

#include <QApplication>
#include <QKeyEvent>

namespace {
    const char * const EnabledPluginsKey = MALIIT_CONFIG_ROOT"onscreen/enabled";
    const char * const ActivePluginKey =   MALIIT_CONFIG_ROOT"onscreen/active";

    const QStringList DefaultEnabledPlugin = QStringList()
                                              << "plugin1" << "dummyim1sv1";

    const QStringList AllEnabledPlugins = QStringList()
                                          << "plugin1" << "dummyim1sv1"
                                          << "plugin1" << "dummyim1sv2"
                                          << "plugin2" << "dummyim2sv1"
                                          << "plugin2" << "dummyim2sv2";

}

// stubs

void MInputContextConnection::sendKeyEvent(const QKeyEvent &,
                                           MInputMethod::EventRequestType )
{
    Q_EMIT keyEventSent();
}

void MInputContextConnection::sendCommitString(const QString &, int ,
                                          int , int )
{
    Q_EMIT stringCommited();
}

// test cases

void Ut_MSubViewWatcher::initTestCase()
{
    static char *argv[1] = { (char *) "ut_mimpluginloader" };
    static int argc = 1;

    QApplication::setStyle("motif"); // prevent loading of LMT style
    app = new QApplication(argc, argv);
}

void Ut_MSubViewWatcher::cleanupTestCase()
{
    delete app;
}

void Ut_MSubViewWatcher::init()
{
    MImSettings enabledPluginsSettings(EnabledPluginsKey);
    enabledPluginsSettings.set(DefaultEnabledPlugin);

    MImSettings activePluginsSettings(EnabledPluginsKey);
    activePluginsSettings.set(DefaultEnabledPlugin);

    onScreenPlugins = new MImOnScreenPlugins;
    connection = new MInputContextConnection;
    subject = new MSubViewWatcher(connection, onScreenPlugins);
}

void Ut_MSubViewWatcher::cleanup()
{
    delete subject;
    subject = 0;
    delete connection;
    connection = 0;
    delete onScreenPlugins;
    onScreenPlugins = 0;
}

void Ut_MSubViewWatcher::testRecording()
{
    QCOMPARE(subject->recordedSubViews.count(), 1);

    MImSettings enabledPluginsSettings(EnabledPluginsKey);
    enabledPluginsSettings.set(AllEnabledPlugins);

    QCOMPARE(subject->recordedSubViews.count(), 1);

    // typing with current active layout should not change anything
    connection->sendKeyEvent(QKeyEvent(QEvent::KeyPress, Qt::Key_Any, Qt::KeyboardModifiers()));
    connection->sendKeyEvent(QKeyEvent(QEvent::KeyRelease, Qt::Key_Any, Qt::KeyboardModifiers()));
    connection->sendCommitString(QString(), 0, 0, 0);
    QCOMPARE(subject->recordedSubViews.count(), 1);

    MImOnScreenPlugins::SubView dummyim1sv2("plugin1", "dummyim1sv2");
    onScreenPlugins->setActiveSubView(dummyim1sv2);
    // there was no typing  events, so this layout is ignored
    QCOMPARE(subject->recordedSubViews.count(), 1);

    MImOnScreenPlugins::SubView dummyim2sv1("plugin2", "dummyim2sv1");
    onScreenPlugins->setActiveSubView(dummyim2sv1);
    connection->sendKeyEvent(QKeyEvent(QEvent::KeyPress, Qt::Key_Any, Qt::KeyboardModifiers()));
    connection->sendKeyEvent(QKeyEvent(QEvent::KeyRelease, Qt::Key_Any, Qt::KeyboardModifiers()));
    QCOMPARE(subject->recordedSubViews.count(), 2);

    MImOnScreenPlugins::SubView dummyim2sv2("plugin2", "dummyim2sv2");
    onScreenPlugins->setActiveSubView(dummyim2sv2);
    connection->sendCommitString(QString(), 0, 0, 0);
    QCOMPARE(subject->recordedSubViews.count(), 3);

    delete subject; // destructor will enforce MImOnScreenPlugins to change settings
    subject = 0;

    QStringList expected = QStringList()
                           << "plugin1" << "dummyim1sv1"
                           << "plugin2" << "dummyim2sv1"
                           << "plugin2" << "dummyim2sv2";

    QStringList enabled;
    Q_FOREACH(const MImOnScreenPlugins::SubView &s, onScreenPlugins->enabledSubViews()) {
        enabled << s.plugin << s.id;
    }

    QCOMPARE(enabled.count(), expected.count());
    for (int n = 1; n <= expected.count() - 1; n+= 2) {
        int index = enabled.indexOf(expected.at(n));
        QVERIFY(index > 0);
        QCOMPARE(enabled.at(index - 1), expected.at(n - 1));
    }
}

QTEST_APPLESS_MAIN(Ut_MSubViewWatcher)
