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

#include <maliit/plugins/windowedsurface.h>

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

namespace Maliit {
namespace Plugins {

class WindowedSurface : public virtual AbstractSurface
{
public:
    WindowedSurface(const SurfacePolicy &policy, const QSharedPointer<WindowedSurface> &parent, QWidget *toplevel)
        : AbstractSurface(),
          mPolicy(policy),
          mParent(parent),
          mToplevel(toplevel)
    {
        QWidget *parentWidget = 0;
        if (parent) {
            parentWidget = parent->mToplevel.data();
        }
        mToplevel->setParent(parentWidget, Qt::Window | Qt::FramelessWindowHint);
        mToplevel->setAttribute(Qt::WA_X11DoNotAcceptFocus);
        mToplevel->setAutoFillBackground(false);
        mToplevel->setBackgroundRole(QPalette::NoRole);

        mToplevel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        setSize(policy.defaultLandscapeSize());
//        mToplevel->hide();
    }

    ~WindowedSurface()
    {
    }

    void show()
    {
        mToplevel->show();
    }

    void hide()
    {
        mToplevel->hide();
    }

    QSize size() const
    {
        return mToplevel->size();
    }

    void setSize(const QSize &size)
    {
        const QSize& desktopSize = QApplication::desktop()->size();

        qWarning() << desktopSize << QPoint(desktopSize.width() - size.width() / 2, desktopSize.height() - size.height());

        switch (mPolicy.position()) {
        case SurfacePositionCenterBottom:
            mToplevel->setGeometry(QRect(QPoint((desktopSize.width() - size.width()) / 2, desktopSize.height() - size.height()), size));
            break;
        default:
            mToplevel->setGeometry(QRect(QPoint(0, 0), size));
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

protected:
    SurfacePolicy mPolicy;
    QSharedPointer<WindowedSurface> mParent;
    QScopedPointer<QWidget> mToplevel;
};

class GraphicsView : public QGraphicsView
{
public:
    GraphicsView()
        : QGraphicsView()
    {
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_X11DoNotAcceptFocus);
        setAutoFillBackground(false);
        setBackgroundRole(QPalette::NoRole);

        setAttribute(Qt::WA_TranslucentBackground);
        viewport()->setAttribute(Qt::WA_TranslucentBackground);
    }

    bool event(QEvent *e)
    {
        if (e->type() == QEvent::WinIdChange) {
    #if QT_VERSION >= 0x050000
            QWindow *win = window()->windowHandle();
            if (win)
                QGuiApplication::platformNativeInterface()->setWindowProperty(win->handle(), "AcceptFocus", QVariant(false));
    #endif
        }
        return QWidget::event(e);
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

class WindowedGraphicsViewSurface : public WindowedSurface, public AbstractGraphicsViewSurface
{
public:
    WindowedGraphicsViewSurface(const SurfacePolicy &policy, const QSharedPointer<WindowedSurface> &parent)
        : WindowedSurface(policy, parent, new GraphicsView),
          AbstractGraphicsViewSurface(),
          mRoot(0)
    {
        view()->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
        view()->setOptimizationFlags(QGraphicsView::DontClipPainter | QGraphicsView::DontSavePainterState);
        view()->setFrameShape(QFrame::NoFrame);
        view()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        QGraphicsScene *scene = new QGraphicsScene(view());
        view()->setScene(scene);
        view()->setSceneRect(QRect(QPoint(0, 0), policy.defaultLandscapeSize()));
        view()->setSceneRect(QRect(QPoint(0, 0), policy.defaultLandscapeSize()));
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

        const QRect &rect(view()->rect());

        if (not mRoot) {
            scene()->addItem(mRoot = new RootItem);
            mRoot->setRect(rect);
            mRoot->show();
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

WindowedSurfaceFactory::WindowedSurfaceFactory()
    : surfaces()
{
}

WindowedSurfaceFactory::~WindowedSurfaceFactory()
{
}

QSharedPointer<AbstractGraphicsViewSurface> WindowedSurfaceFactory::createGraphicsViewSurface(const SurfacePolicy &policy, const QSharedPointer<AbstractSurface> &parent)
{
    QSharedPointer<WindowedSurface> defaultSurfaceParent(qSharedPointerDynamicCast<WindowedSurface>(parent));
    QSharedPointer<WindowedGraphicsViewSurface> newSurface(new WindowedGraphicsViewSurface(policy, defaultSurfaceParent));
    surfaces.push_back(newSurface);
    return newSurface;
}

QSharedPointer<AbstractWidgetSurface> WindowedSurfaceFactory::createWidgetSurface(const SurfacePolicy &policy, const QSharedPointer<AbstractSurface> &parent)
{
    return QSharedPointer<AbstractWidgetSurface>();
}

} // namespace Server
} // namespace Maliit
