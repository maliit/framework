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
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QInputContext>
#endif
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QWidget>

#include <tr1/functional>
#include <vector>

#include <minputmethodhost.h>
#include <windowgroup.h>
#include <minputcontextconnection.h>

#include <maliit/plugins/abstractpluginsetting.h>

namespace MaliitTestUtils {

    class RemoteWindow : public QWidget
    {
    public:
        explicit RemoteWindow(QWidget *p = 0, Qt::WindowFlags f = 0);

        void paintEvent(QPaintEvent *);
    };

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    template <typename T>
    class EventSpyInputContext : public QInputContext, public std::vector<T>
    {
    public:
        typedef std::tr1::function<T (const QEvent *event)> TransformFunction;

        EventSpyInputContext(TransformFunction newTransform) : transform(newTransform) {}

        virtual QString identifierName() { return QString::fromLatin1("EventSpyInputContext"); }
        virtual bool isComposing() const { return false; }
        virtual QString language() { return QString::fromLatin1("EN"); }
        virtual void reset() {}

    protected:
        virtual bool filterEvent(const QEvent *event) {
            this->push_back(transform(event));
            return true;
        }

    private:
        const TransformFunction transform;
    };
#endif

    class TestGraphicsViewSurface : public QGraphicsView
    {
    public:
        TestGraphicsViewSurface()
            : QGraphicsView(new QGraphicsScene)
            , mRootItem(new QGraphicsRectItem)
        {
            scene()->addItem(mRootItem.data());
        }

        QGraphicsItem *root() const { return mRootItem.data(); }

    private:
        QScopedPointer<QGraphicsItem> mRootItem;
    };

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

        TestInputMethodHost(MIndicatorServiceClient &client, const QString &plugin, const QString &description)
            : MInputMethodHost(QSharedPointer<MInputContextConnection>(new MInputContextConnection), 0, client, QSharedPointer<Maliit::WindowGroup>(new Maliit::WindowGroup), plugin, description)
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

// For cases where we need to run code _before_ QApplication is created
#define MALIIT_TESTUTILS_GUI_MAIN_WITH_SETUP(TestObject, setupFunc) \
int main(int argc, char *argv[]) \
{ \
    setupFunc();\
    QApplication app(argc, argv);\
    Q_UNUSED(app);\
    TestObject tc;\
    return QTest::qExec(&tc, argc, argv);\
}

#endif // UTILS_H__
