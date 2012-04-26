/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#include "windowedsurface.h"

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
    WindowedSurface(AbstractSurface::Options options, const QSharedPointer<WindowedSurface> &parent, QWidget *toplevel)
        : AbstractSurface(),
          mOptions(options),
          mParent(parent),
          mToplevel(toplevel),
          mActive(false),
          mVisible(false)
    {
        QWidget *parentWidget = 0;
        if (parent) {
            parentWidget = parent->mToplevel.data();
        }
        mToplevel->setParent(parentWidget, static_cast<Qt::WindowFlags>(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint));
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
        const QSize& desktopSize = QApplication::desktop()->size();

        if (mOptions & PositionCenterBottom) {
            mToplevel->setGeometry(QRect(QPoint((desktopSize.width() - size.width()) / 2, desktopSize.height() - size.height()), size));
        } else {
            mToplevel->resize(size);
        }
    }

    QPoint relativePosition() const
    {
        return mToplevel->pos();
    }

    void setRelativePosition(const QPoint &position)
    {
        QPoint parentPosition;
        if (mParent) {
            parentPosition = mParent->relativePosition();
        }
        mToplevel->move(parentPosition + position);
    }


    QSharedPointer<AbstractSurface> parent() const
    {
        return mParent;
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

private:
    void updateVisibility()
    {
        if (mActive && mVisible) {
            mToplevel->show();
        } else {
            mToplevel->hide();
        }
    }

protected:
    Options mOptions;
    QSharedPointer<WindowedSurface> mParent;
    QScopedPointer<QWidget> mToplevel;
    bool mActive;
    bool mVisible;
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
    WindowedGraphicsViewSurface(AbstractSurface::Options options, const QSharedPointer<WindowedSurface> &parent)
        : WindowedSurface(options, parent, new GraphicsView),
          AbstractGraphicsViewSurface(),
          mRoot(0)
    {
        QGraphicsView *view = static_cast<QGraphicsView*>(mToplevel.data());
        view->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
        view->setOptimizationFlags(QGraphicsView::DontClipPainter | QGraphicsView::DontSavePainterState);
        view->setFrameShape(QFrame::NoFrame);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        QGraphicsScene *scene = new QGraphicsScene(view);
        view->setScene(scene);
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
    WindowedWidgetSurface(AbstractSurface::Options options, const QSharedPointer<WindowedSurface> &parent)
        : WindowedSurface(options, parent, new QWidget),
          AbstractWidgetSurface()
    {}

    QWidget* widget() const {
        return mToplevel.data();
    }
};

WindowedSurfaceFactory::WindowedSurfaceFactory()
    : surfaces()
{
}

WindowedSurfaceFactory::~WindowedSurfaceFactory()
{
}

bool WindowedSurfaceFactory::supported(Maliit::Plugins::AbstractSurface::Options options) const
{
    return options & AbstractSurface::TypeGraphicsView;
}

QSharedPointer<AbstractSurface> WindowedSurfaceFactory::create(AbstractSurface::Options options, const QSharedPointer<AbstractSurface> &parent)
{
    QSharedPointer<WindowedSurface> defaultSurfaceParent(qSharedPointerDynamicCast<WindowedSurface>(parent));
    if (options & Maliit::Plugins::AbstractSurface::TypeGraphicsView) {
        QSharedPointer<WindowedSurface> newSurface(new WindowedGraphicsViewSurface(options, defaultSurfaceParent));
        surfaces.push_back(newSurface);
        return newSurface;
    } else if (options & Maliit::Plugins::AbstractSurface::TypeWidget) {
        QSharedPointer<WindowedSurface> newSurface(new WindowedWidgetSurface(options, defaultSurfaceParent));
        surfaces.push_back(newSurface);
        return newSurface;
    }
    return QSharedPointer<AbstractSurface>();
}

void WindowedSurfaceFactory::activate()
{
    Q_FOREACH(QWeakPointer<WindowedSurface> weakSurface, surfaces) {
        QSharedPointer<WindowedSurface> surface = weakSurface.toStrongRef();
        if (surface)
            surface->setActive(true);
    }
}

void WindowedSurfaceFactory::deactivate()
{
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

} // namespace Server
} // namespace Maliit
