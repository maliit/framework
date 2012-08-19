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
#include <minputcontextconnection.h>

#include <maliit/plugins/abstractsurface.h>
#include <maliit/plugins/abstractsurfacefactory.h>
#include <maliit/plugins/abstractwidgetssurface.h>
#include <maliit/plugins/abstractpluginsetting.h>

namespace MaliitTestUtils {

class RemoteWindow
    : public QWidget
{
public:
    explicit RemoteWindow(QWidget *parent = 0,
                          Qt::WindowFlags flags = 0);

    void paintEvent(QPaintEvent *ev);
};

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
template <typename T>
class EventSpyInputContext
    : public QInputContext, public std::vector<T>
{
public:
    typedef std::tr1::function<T (const QEvent *event)> TransformFunction;

    EventSpyInputContext(TransformFunction newTransform)
        : transform(newTransform) {}

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

using Maliit::Plugins::AbstractSurface;
using Maliit::Plugins::SharedSurface;
using Maliit::Plugins::AbstractWidgetSurface;
using Maliit::Plugins::AbstractGraphicsViewSurface;

class TestWidgetSurface
    : public AbstractWidgetSurface
{
public:
    TestWidgetSurface() {}

    void show() {}
    void hide() {}

    QSize size() const { return QSize(); }
    void setSize(const QSize&) {}

    QPoint relativePosition() const { return QPoint(); }
    void setRelativePosition(const QPoint &) {}

    SharedSurface parent() const { return SharedSurface(); }

    QPoint translateEventPosition(const QPoint &event_position,
                                  const SharedSurface & = SharedSurface()) const { return event_position; }

    QWidget *widget() const { return 0; }
};

class TestGraphicsViewSurface : public AbstractGraphicsViewSurface
{
public:
    TestGraphicsViewSurface()
        : m_graphics_view(new QGraphicsView(new QGraphicsScene))
        , m_root_item(new QGraphicsRectItem)
    {
        m_graphics_view->scene()->addItem(m_root_item.data());
    }

    void show() {}
    void hide() {}

    QSize size() const { return QSize(); }
    void setSize(const QSize&) {}

    QPoint relativePosition() const { return QPoint(); }
    void setRelativePosition(const QPoint &) {}

    SharedSurface parent() const { return SharedSurface(); }

    QPoint translateEventPosition(const QPoint &event_position,
                                  const SharedSurface & = SharedSurface()) const { return event_position; }

    QGraphicsScene *scene() const { return m_graphics_view->scene(); }
    QGraphicsView *view() const { return m_graphics_view.data(); }

    QGraphicsItem *root() const { return m_root_item.data(); }
    void clear() {}
private:
    QScopedPointer<QGraphicsView> m_graphics_view;
    QScopedPointer<QGraphicsItem> m_root_item;
};

class TestSurfaceFactory
    : public Maliit::Plugins::AbstractSurfaceFactory
{
public:
    TestSurfaceFactory() {}

    QSize screenSize() const { return QSize(); }

    bool supported(AbstractSurface::Options options) const { return options & (AbstractSurface::TypeGraphicsView | AbstractSurface::TypeWidget); }

    SharedSurface create(AbstractSurface::Options options,
                         const SharedSurface &) {
        if (options & AbstractSurface::TypeGraphicsView)
            return SharedSurface(new TestGraphicsViewSurface);
        else if (options & AbstractSurface::TypeWidget)
            return SharedSurface(new TestWidgetSurface);
        else
            return SharedSurface();
    }
};

class TestPluginSetting
    : public Maliit::Plugins::AbstractPluginSetting
{
public:
    TestPluginSetting(const QString &key)
        : m_setting_key(key) {}

    QString key() const { return m_setting_key; }

    QVariant value() const { return QVariant(); }
    QVariant value(const QVariant &def) const { return def; }

    void set(const QVariant &val) { Q_UNUSED(val); }
    void unset() {}

private:
    QString m_setting_key;
};

class TestInputMethodHost
    : public MInputMethodHost
{
public:
    QString last_commit;
    int send_commit_count;

    QString last_preedit;
    int send_preedit_count;

    TestInputMethodHost(MIndicatorServiceClient &client,
                        const QString &plugin,
                        const QString &description)
        : MInputMethodHost(std::tr1::shared_ptr<MInputContextConnection>(new MInputContextConnection), 0, client, new TestSurfaceFactory, plugin, description)
        , send_commit_count(0)
        , send_preedit_count(0)
    {}

    void sendCommitString(const QString &string,
                          int start,
                          int length,
                          int cursorPos)
    {
        last_commit = string;
        ++send_commit_count;
        MInputMethodHost::sendCommitString(string, start, length, cursorPos);
    }

    void sendPreeditString(const QString &string,
                           const QList<Maliit::PreeditTextFormat> &preeditFormats,
                           int start,
                           int length,
                           int cursorPos)
    {
        last_preedit = string;
        ++send_preedit_count;
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

} // namespace MaliitTestUtils

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
