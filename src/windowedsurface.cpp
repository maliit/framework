/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
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
#include "windowedsurface_p.h"
#include "windowedsurfacefactory_p.h"

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

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QGuiApplication>
#include <QtGui/5.0.0/QtGui/qpa/qplatformnativeinterface.h>
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

namespace {
    const Qt::WindowFlags g_window_flags =
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Qt::WindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint | Qt::WindowDoesNotAcceptFocus);
#else
    Qt::WindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
#endif
}

WindowedSurface::WindowedSurface(WindowedSurfaceFactory *factory,
                                 AbstractSurface::Options options,
                                 const QSharedPointer<WindowedSurface> &parent,
                                 QWidget *toplevel)
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
    mToplevel->setParent(parentWidget, g_window_flags);

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    mToplevel->setAttribute(Qt::WA_X11DoNotAcceptFocus);
#endif
    mToplevel->setAutoFillBackground(false);
    mToplevel->setBackgroundRole(QPalette::NoRole);

    mToplevel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    updateVisibility();
}

WindowedSurface::~WindowedSurface()
{
}

void WindowedSurface::show()
{
    mVisible = true;
    updateVisibility();
}

void WindowedSurface::hide()
{
    mVisible = false;
    updateVisibility();
}

QSize WindowedSurface::size() const
{
    return mToplevel->size();
}

void WindowedSurface::setSize(const QSize &size)
{
    const QSize& desktopSize = QApplication::desktop()->screenGeometry().size();

    if (isWindow()) {
        // stand-alone Maliit server
        if (mOptions & PositionCenterBottom) {
            mToplevel->setGeometry(QRect(QPoint((desktopSize.width() - size.width()) / 2,
                                                desktopSize.height() - size.height()), size));
        } else {
            mToplevel->resize(size);
        }
    } else {
        // application-hosted Maliit server
        mToplevel->resize(size);
    }
    mFactory->updateInputMethodArea();
}

QPoint WindowedSurface::relativePosition() const
{
    return mRelativePosition;
}

void WindowedSurface::setRelativePosition(const QPoint &position)
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

QSharedPointer<AbstractSurface> WindowedSurface::parent() const
{
    return mParent;
}

QPoint WindowedSurface::translateEventPosition(const QPoint &eventPosition,
                                               const QSharedPointer<AbstractSurface> &eventSurface) const
{
    if (!eventSurface)
        return eventPosition;

    QSharedPointer<WindowedSurface> windowedSurface = qSharedPointerDynamicCast<WindowedSurface>(eventSurface);
    if (!windowedSurface)
        return QPoint();

    return -mToplevel->pos() + eventPosition + windowedSurface->mToplevel->pos();
}

void WindowedSurface::setActive(bool active)
{
    mActive = active;
    updateVisibility();
}

void WindowedSurface::applicationFocusChanged(WId winId)
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

QRegion WindowedSurface::inputMethodArea()
{
    if (!mToplevel->isVisible())
        return QRegion();

    return QRegion(mToplevel->geometry());
}

void WindowedSurface::updateVisibility()
{
    mToplevel->setVisible(mActive && mVisible);
    mFactory->updateInputMethodArea();
}

bool WindowedSurface::isWindow() const
{
    return mToplevel->isWindow();
}

QPoint WindowedSurface::mapToGlobal(const QPoint &pos) const
{
    return mToplevel->mapToGlobal(pos);
}

class GraphicsView : public QGraphicsView
{
public:
    GraphicsView()
        : QGraphicsView()
    {
        setWindowFlags(g_window_flags);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        setAttribute(Qt::WA_X11DoNotAcceptFocus);
#endif
        // Fixes: MALIIT#194 - Maliit can not input when QML viewer is set to
        // full screen on QWS without x11
        setAttribute(Qt::WA_ShowWithoutActivating);

        setAutoFillBackground(false);
        setBackgroundRole(QPalette::NoRole);
        setBackgroundBrush(Qt::transparent);

        // This is a workaround for non-compositing window managers. Apparently
        // setting this attribute while using such WMs may cause garbled
        // painting of VKB.
#ifndef DISABLE_TRANSLUCENT_BACKGROUND_HINT
        setAttribute(Qt::WA_TranslucentBackground);
#endif
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

WindowedSurfaceFactoryPrivate::WindowedSurfaceFactoryPrivate(WindowedSurfaceFactory *factory)
    : QObject()
    , q_ptr(factory)
    , surfaces()
    , active(false)
{
    connect(QApplication::desktop(), SIGNAL(resized(int)),
            this, SLOT(screenResized(int)));
}

void WindowedSurfaceFactoryPrivate::screenResized(int)
{
    Q_Q(WindowedSurfaceFactory);

    Q_FOREACH(QWeakPointer<WindowedSurface> weakSurface, surfaces) {
        QSharedPointer<WindowedSurface> surface = weakSurface.toStrongRef();
        if (surface) {
            surface->setSize(surface->size());
            if (surface->parent()) {
                surface->setRelativePosition(surface->relativePosition());
            }
        }
    }
    Q_EMIT q->screenSizeChanged(q->screenSize());
}

// Windowed Surface Factory

WindowedSurfaceFactory::WindowedSurfaceFactory()
    : AbstractSurfaceFactory()
    , d_ptr(new WindowedSurfaceFactoryPrivate(this))
{
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
    Q_D(WindowedSurfaceFactory);

    d->active = true;

    Q_FOREACH(QWeakPointer<WindowedSurface> weakSurface, d->surfaces) {
        QSharedPointer<WindowedSurface> surface = weakSurface.toStrongRef();
        if (surface)
            surface->setActive(true);
    }
}

void WindowedSurfaceFactory::deactivate()
{
    Q_D(WindowedSurfaceFactory);

    d->active = false;

    Q_FOREACH(QWeakPointer<WindowedSurface> weakSurface, d->surfaces) {
        QSharedPointer<WindowedSurface> surface = weakSurface.toStrongRef();
        if (surface)
            surface->setActive(false);
    }
}

void WindowedSurfaceFactory::applicationFocusChanged(WId winId)
{
    Q_D(WindowedSurfaceFactory);

    Q_FOREACH(QWeakPointer<WindowedSurface> weakSurface, d->surfaces) {
        QSharedPointer<WindowedSurface> surface = weakSurface.toStrongRef();
        if (surface) {
            surface->applicationFocusChanged(winId);
        }
    }
}

void WindowedSurfaceFactory::updateInputMethodArea()
{
    Q_D(WindowedSurfaceFactory);

    if (!d->active)
        return;

    QRegion inputMethodArea;

    Q_FOREACH(QWeakPointer<WindowedSurface> weakSurface, d->surfaces) {
        QSharedPointer<WindowedSurface> surface = weakSurface.toStrongRef();
        if (surface && !surface->parent()) {
            inputMethodArea |= surface->inputMethodArea();
        }
    }

    Q_EMIT inputMethodAreaChanged(inputMethodArea);
}

} // namespace Server
} // namespace Maliit
