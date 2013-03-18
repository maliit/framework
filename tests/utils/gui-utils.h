/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef GUI_UTILS_H__
#define GUI_UTILS_H__

#include <QtGlobal>

#include <minputmethodhost.h>
#include <windowgroup.h>
#include <minputcontextconnection.h>
#include <unknownplatform.h>

#include <maliit/plugins/abstractpluginsetting.h>

namespace MaliitTestUtils {

    class TestPluginSetting : public Maliit::Plugins::AbstractPluginSetting
    {
    public:
        TestPluginSetting(const QString &key) : settingKey(key) {}

        QString key() const { return settingKey; }

        QVariant value() const { return QVariant(); }
        QVariant value(const QVariant &def) const { return def; }

        void set(const QVariant &val) { Q_UNUSED(val); }
        void unset() {}

    private:
        QString settingKey;
    };

    class TestInputMethodHost
        : public MInputMethodHost
    {
    public:
        QString lastCommit;
        int sendCommitCount;

        QString lastPreedit;
        int sendPreeditCount;

        TestInputMethodHost(const QString &plugin, const QString &description)
            : MInputMethodHost(QSharedPointer<MInputContextConnection>(new MInputContextConnection),
                               0,
                               QSharedPointer<Maliit::WindowGroup>(new Maliit::WindowGroup(QSharedPointer<Maliit::AbstractPlatform>(new Maliit::UnknownPlatform))),
                               plugin,
                               description)
            , sendCommitCount(0)
            , sendPreeditCount(0)
        {}

        void sendCommitString(const QString &string,
                              int start, int length, int cursorPos)
        {
            lastCommit = string;
            ++sendCommitCount;
            MInputMethodHost::sendCommitString(string, start, length, cursorPos);
        }

        void sendPreeditString(const QString &string,
                               const QList<Maliit::PreeditTextFormat> &preeditFormats,
                               int start, int length, int cursorPos)
        {
            lastPreedit = string;
            ++sendPreeditCount;
            MInputMethodHost::sendPreeditString(string, preeditFormats, start, length, cursorPos);
        }

        AbstractPluginSetting *registerPluginSetting(const QString &key,
                                                     const QString &description,
                                                     Maliit::SettingEntryType type,
                                                     const QVariantMap &attributes)
        {
            Q_UNUSED(description);
            Q_UNUSED(type);
            Q_UNUSED(attributes);

            return new TestPluginSetting(key);
        }
    };

}

// For cases where we need to run code _before_ QGuiApplication is created
#define MALIIT_TESTUTILS_GUI_MAIN_WITH_SETUP(TestObject, setupFunc) \
int main(int argc, char *argv[]) \
{ \
    setupFunc();\
    QGuiApplication app(argc, argv);\
    Q_UNUSED(app);\
    TestObject tc;\
    return QTest::qExec(&tc, argc, argv);\
}

#endif // UTILS_H__
