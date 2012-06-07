/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * Copyright (C) 2012 One Laptop per Child Association
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "windowedsurface.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include "mimdummyinputcontext.h"
#endif

#include "mimapphostedserverlogic.h"

#include <maliit/plugins/abstractwidgetssurface.h>

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QWidget>

#if QT_VERSION >= 0x050000
#include <QGuiApplication>
#include <QPlatformNativeInterface>
#include <QVariant>
#include <QWindow>
#endif

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>
#endif

using Maliit::Plugins::AbstractSurface;

namespace Maliit {
namespace Server {

class WindowedSurface : public virtual Maliit::Plugins::AbstractSurface
{
public:
    WindowedSurface(WindowedSurfaceFactory *factory, AbstractSurface::Options options, const QSharedPointer<WindowedSurface> &parent, QWidget *toplevel)
        : AbstractSurface(),
          mFactory(factory),
          mOptions(options),
          mParent(parent),
          mToplevel(toplevel),
          mActive(false),
          mVisible(false),
          mRelativePosition()
    {
        QWidget *parentWidget = 0;
        if (parent) {
            parentWidget = parent->mToplevel.data();
        }
        mToplevel->setParent(parentWidget, static_cast<Qt::WindowFlags>(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint));
        mToplevel->setAttribute(Qt::WA_X11DoNotAcceptFocus);
        mToplevel->setAutoFillBackground(false);
        mToplevel->setBackgroundRole(QPalette::NoRole);

        mToplevel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        updateVisibility();
    }

    ~WindowedSurface()
    {
    }

    void show()
    {
        mVisible = true;
        updateVisibility();
    }

    void hide()
    {
        mVisible = false;
        updateVisibility();
    }

    QSize size() const
    {
        return mToplevel->size();
    }

    void setSize(const QSize &size)
    {
        const QSize& desktopSize = QApplication::desktop()->screenGeometry().size();

        if (isWindow()) {
            // stand-alone Maliit server
            if (mOptions & PositionCenterBottom) {
                mToplevel->setGeometry(QRect(QPoint((desktopSize.width() - size.width()) / 2, desktopSize.height() - size.height()), size));
            } else {
                mToplevel->resize(size);
            }
        } else {
            // application-hosted Maliit server
            mToplevel->resize(size);
        }
        mFactory->updateInputMethodArea();
    }

    QPoint relativePosition() const
    {
        return mRelativePosition;
    }

    void setRelativePosition(const QPoint &position)
    {
        mRelativePosition = position;
        QPoint parentPosition(0, 0);
        if (mParent) {
            if (isWindow() && !mParent->isWindow()) {
                parentPosition = mParent->mapToGlobal(QPoint(0, 0));
            } else if (!isWindow() && mParent->isWindow()) {
                // do nothing
            } else {
                parentPosition = mParent->mToplevel->pos();
            }
        }
        mToplevel->move(parentPosition + mRelativePosition);
        mFactory->updateInputMethodArea();
    }


    QSharedPointer<AbstractSurface> parent() const
    {
        return mParent;
    }

    QPoint translateEventPosition(const QPoint &eventPosition, const QSharedPointer<AbstractSurface> &eventSurface = QSharedPointer<AbstractSurface>()) const
    {
        if (!eventSurface)
            return eventPosition;

        QSharedPointer<WindowedSurface> windowedSurface = qSharedPointerDynamicCast<WindowedSurface>(eventSurface);
        if (!windowedSurface)
            return QPoint();

        return -mToplevel->pos() + eventPosition + windowedSurface->mToplevel->pos();
    }

    void setActive(bool active)
    {
        mActive = active;
        updateVisibility();
    }


    void applicationFocusChanged(WId winId)
    {
        if (mParent)
            return;
#ifdef Q_WS_X11
        XSetTransientForHint(QX11Info::display(),
                             mToplevel->window()->effectiveWinId(),
                             winId);
#else
        Q_UNUSED(winId);
#endif
    }

    QRegion inputMethodArea()
    {
        if (!mToplevel->isVisible())
            return QRegion();

        return QRegion(mToplevel->geometry());
    }

private:
    void updateVisibility()
    {
        mToplevel->setVisible(mActive && mVisible);
        mFactory->updateInputMethodArea();
    }

protected:
    bool isWindow() const { return mToplevel->isWindow(); }
    QPoint mapToGlobal(const QPoint &pos) const { return mToplevel->mapToGlobal(pos); }

    WindowedSurfaceFactory *mFactory;
    Options mOptions;
    QSharedPointer<WindowedSurface> mParent;
    QScopedPointer<QWidget> mToplevel;
    bool mActive;
    bool mVisible;
    QPoint mRelativePosition;
};

class GraphicsView : public QGraphicsView
{
public:
    GraphicsView()
        : QGraphicsView()
    {
        setWindowFlags(static_cast<Qt::WindowFlags>(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint));
        setAttribute(Qt::WA_X11DoNotAcceptFocus);
        setAutoFillBackground(false);
        setBackgroundRole(QPalette::NoRole);
        setBackgroundBrush(Qt::transparent);

        setAttribute(Qt::WA_TranslucentBackground);
        viewport()->setAutoFillBackground(false);
    }
};

class RootItem
    : public QGraphicsItem
{
private:
    QRectF m_rect;

public:
    explicit RootItem(QGraphicsItem *parent = 0)
        : QGraphicsItem(parent)
        , m_rect()
    {
        setFlag(QGraphicsItem::ItemHasNoContents);
    }

    void setRect(const QRectF &rect)
    {
        m_rect = rect;
    }

    virtual QRectF boundingRect() const
    {
        return m_rect;
    }

    virtual void paint(QPainter *,
                       const QStyleOptionGraphicsItem *,
                       QWidget *)
    {}
};

class WindowedGraphicsViewSurface : public WindowedSurface, public Maliit::Plugins::AbstractGraphicsViewSurface
{
public:
    WindowedGraphicsViewSurface(WindowedSurfaceFactory *factory, AbstractSurface::Options options, const QSharedPointer<WindowedSurface> &parent)
        : WindowedSurface(factory, options, parent, new GraphicsView),
          AbstractGraphicsViewSurface(),
          mRoot(0)
    {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        MIMDummyInputContext dummy;
#endif
        QGraphicsView *view = static_cast<QGraphicsView*>(mToplevel.data());
        view->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
        view->setOptimizationFlags(QGraphicsView::DontClipPainter | QGraphicsView::DontSavePainterState);
        view->setFrameShape(QFrame::NoFrame);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        // Calling QGraphicsView::setScene() indirectly calls QWidget::inputContext() on the view.  If there isn't
        // an input context set on the widget, this calls QApplication::inputContext(), which leads to infinite
        // recursion if surface creation happens during input method creation and QT_IM_MODULE is set (for example
        // when embedding maliit-server in the application)
        view->setInputContext(&dummy);
#endif
        QGraphicsScene *scene = new QGraphicsScene(view);
        view->setScene(scene);

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        view->setInputContext(0);
#endif
    }

    ~WindowedGraphicsViewSurface() {}

    QGraphicsScene *scene() const
    {
        return view()->scene();
    }

    QGraphicsView *view() const
    {
        return static_cast<QGraphicsView*>(mToplevel.data());
    }

    void show()
    {
        WindowedSurface::show();

        const QRect rect(QPoint(), mToplevel->size());

        if (not mRoot) {
            scene()->addItem(mRoot = new RootItem);
            mRoot->setRect(rect);
            mRoot->show();
        }
    }

    void setSize(const QSize &size)
    {
        WindowedSurface::setSize(size);

        view()->setSceneRect(QRect(QPoint(), mToplevel->size()));
        if (mRoot) {
            mRoot->setRect(QRect(QPoint(), mToplevel->size()));
        }
    }

    void clear()
    {
        mRoot = 0;
        scene()->clear();
    }

    QGraphicsItem *root() const
    {
        return mRoot;
    }

private:
    RootItem *mRoot;
};

class WindowedWidgetSurface : public WindowedSurface, public Maliit::Plugins::AbstractWidgetSurface
{
public:
    WindowedWidgetSurface(WindowedSurfaceFactory *factory, AbstractSurface::Options options, const QSharedPointer<WindowedSurface> &parent)
        : WindowedSurface(factory, options, parent, new QWidget),
          AbstractWidgetSurface()
    {}

    QWidget* widget() const {
        return mToplevel.data();
    }
};

WindowedSurfaceFactory::WindowedSurfaceFactory()
    : AbstractSurfaceFactory()
    , surfaces()
    , mActive(false)
{
    connect(QApplication::desktop(), SIGNAL(resized(int)),
            this, SLOT(screenResized(int)));
}

WindowedSurfaceFactory::~WindowedSurfaceFactory()
{
}

QSize WindowedSurfaceFactory::screenSize() const
{
    return QApplication::desktop()->screenGeometry().size();
}

bool WindowedSurfaceFactory::supported(Maliit::Plugins::AbstractSurface::Options options) const
{
    return options & AbstractSurface::TypeGraphicsView;
}

QSharedPointer<AbstractSurface> WindowedSurfaceFactory::create(AbstractSurface::Options options, const QSharedPointer<AbstractSurface> &parent)
{
    QSharedPointer<WindowedSurface> defaultSurfaceParent(qSharedPointerDynamicCast<WindowedSurface>(parent));
    if (options & Maliit::Plugins::AbstractSurface::TypeGraphicsView) {
        QSharedPointer<WindowedGraphicsViewSurface> newSurface(new WindowedGraphicsViewSurface(this, options, defaultSurfaceParent));
        surfaces.push_back(newSurface);
        Q_EMIT surfaceWidgetCreated(newSurface->view(), options);
        return newSurface;
    } else if (options & Maliit::Plugins::AbstractSurface::TypeWidget) {
        QSharedPointer<WindowedWidgetSurface> newSurface(new WindowedWidgetSurface(this, options, defaultSurfaceParent));
        surfaces.push_back(newSurface);
        Q_EMIT surfaceWidgetCreated(newSurface->widget(), options);
        return newSurface;
    }
    return QSharedPointer<AbstractSurface>();
}

void WindowedSurfaceFactory::activate()
{
    mActive = true;

    Q_FOREACH(QWeakPointer<WindowedSurface> weakSurface, surfaces) {
        QSharedPointer<WindowedSurface> surface = weakSurface.toStrongRef();
        if (surface)
            surface->setActive(true);
    }
}

void WindowedSurfaceFactory::deactivate()
{
    mActive = false;

    Q_FOREACH(QWeakPointer<WindowedSurface> weakSurface, surfaces) {
        QSharedPointer<WindowedSurface> surface = weakSurface.toStrongRef();
        if (surface)
            surface->setActive(false);
    }
}

void WindowedSurfaceFactory::applicationFocusChanged(WId winId)
{
    Q_FOREACH(QWeakPointer<WindowedSurface> weakSurface, surfaces) {
        QSharedPointer<WindowedSurface> surface = weakSurface.toStrongRef();
        if (surface) {
            surface->applicationFocusChanged(winId);
        }
    }
}

void WindowedSurfaceFactory::screenResized(int)
{
    Q_FOREACH(QWeakPointer<WindowedSurface> weakSurface, surfaces) {
        QSharedPointer<WindowedSurface> surface = weakSurface.toStrongRef();
        if (surface) {
            surface->setSize(surface->size());
            if (surface->parent()) {
                surface->setRelativePosition(surface->relativePosition());
            }
        }
    }
    Q_EMIT screenSizeChanged(screenSize());
}

void WindowedSurfaceFactory::updateInputMethodArea()
{
    if (!mActive)
        return;

    QRegion inputMethodArea;

    Q_FOREACH(QWeakPointer<WindowedSurface> weakSurface, surfaces) {
        QSharedPointer<WindowedSurface> surface = weakSurface.toStrongRef();
        if (surface && !surface->parent()) {
            inputMethodArea |= surface->inputMethodArea();
        }
    }

    Q_EMIT inputMethodAreaChanged(inputMethodArea);
}

} // namespace Server
} // namespace Maliit
