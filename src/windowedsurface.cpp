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
#include <QVariant>
#include <QWindow>
#include <qpa/qplatformnativeinterface.h>
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
    Qt::WindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint
                    | Qt::WindowDoesNotAcceptFocus);
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

    mToplevel->installEventFilter(this);
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
    if (isWindow()) {
        // stand-alone Maliit server
        if (mOptions & PositionCenterBottom) {
            const QSize desktopSize = QApplication::desktop()->screenGeometry().size();

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

bool WindowedSurface::eventFilter(QObject *, QEvent *event)
{
#ifdef HAVE_WAYLAND
    if (event->type() == QEvent::WinIdChange) {
        mSurface = static_cast<struct input_panel_surface *>(mFactory->getInputPanelSurface(mToplevel->windowHandle()));
        input_panel_surface_set_toplevel(mSurface);
    }
#else
    Q_UNUSED(event);
#endif

    return false;
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
    WindowedGraphicsViewSurface(WindowedSurfaceFactory *factory, AbstractSurface::Options options,
                                const QSharedPointer<WindowedSurface> &parent)
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

    QWidget *widget() const
    {
        return view();
    }

private:
    RootItem *mRoot;
};

class WindowedWidgetSurface : public WindowedSurface, public Maliit::Plugins::AbstractWidgetSurface
{
public:
    WindowedWidgetSurface(WindowedSurfaceFactory *factory, AbstractSurface::Options options,
                          const QSharedPointer<WindowedSurface> &parent)
        : WindowedSurface(factory, options, parent, new QWidget),
          AbstractWidgetSurface()
    {}

    QWidget* widget() const {
        return mToplevel.data();
    }
};

#ifdef HAVE_WAYLAND
namespace {

void registryGlobal(void *data,
                    wl_registry *registry,
                    uint32_t name,
                    const char *interface,
                    uint32_t version)
{
    WindowedSurfaceFactoryPrivate *d = static_cast<WindowedSurfaceFactoryPrivate *>(data);

    Q_UNUSED(registry);
    d->handleRegistryGlobal(name, interface, version);
}

void registryGlobalRemove(void *data,
                          wl_registry *registry,
                          uint32_t name)
{
    WindowedSurfaceFactoryPrivate *d = static_cast<WindowedSurfaceFactoryPrivate *>(data);

    Q_UNUSED(registry);
    d->handleRegistryGlobalRemove(name);
}

const wl_registry_listener maliit_registry_listener = {
    registryGlobal,
    registryGlobalRemove
};

void outputGeometry(void *data,
                    struct wl_output *output,
                    int32_t x,
                    int32_t y,
                    int32_t physical_width,
                    int32_t physical_height,
                    int32_t subpixel,
                    const char *make,
                    const char *model,
                    int32_t transform) {
    WindowedSurfaceFactoryPrivate *d = static_cast<WindowedSurfaceFactoryPrivate *>(data);

    Q_UNUSED(output);
    d->handleOutputGeometry(x, y, physical_width, physical_height, subpixel, make,
                            model, transform);
}

void outputMode(void *data,
                struct wl_output *output,
                uint32_t flags,
                int32_t width,
                int32_t height,
                int32_t refresh) {
    WindowedSurfaceFactoryPrivate *d = static_cast<WindowedSurfaceFactoryPrivate *>(data);

    Q_UNUSED(output);
    d->handleOutputMode(flags, width, height, refresh);
}

const wl_output_listener maliit_output_listener = {
    outputGeometry,
    outputMode
};

} // unnamed namespace
#endif

WindowedSurfaceFactoryPrivate::WindowedSurfaceFactoryPrivate(WindowedSurfaceFactory *factory)
    : QObject()
    , q_ptr(factory)
    , surfaces()
    , active(false)
#ifdef HAVE_WAYLAND
    , registry(0)
    , output(0)
    , panel(0)
    , output_configured(false)
#endif
{
    connect(QApplication::desktop(), SIGNAL(resized(int)),
            this, SLOT(screenResized(int)));

#ifdef HAVE_WAYLAND
    wl_display *display = static_cast<wl_display *>(QGuiApplication::platformNativeInterface()->nativeResourceForIntegration("display"));
    if (!display) {
        qCritical() << __PRETTY_FUNCTION__ << "Failed to get a display.";
        return;
    }
    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &maliit_registry_listener, this);
    // QtWayland will do dispatching for us.
#endif
}

#ifdef HAVE_WAYLAND
void WindowedSurfaceFactoryPrivate::handleRegistryGlobal(uint32_t name,
                                                         const char *interface,
                                                         uint32_t version)
{
    Q_UNUSED(version);

    if (!strcmp(interface, "input_panel")) {
        panel = static_cast<input_panel *>(wl_registry_bind(registry, name, &input_panel_interface, 1));
    } else if (!strcmp(interface, "wl_output")) {
        if (output) {
            // Ignoring the event - we already have an output.
            // TODO: Later we will want to store those outputs as well
            // to be able to choose where the input method plugin is
            // shown.
            return;
        }

        output = static_cast<wl_output *>(wl_registry_bind(registry, name, &wl_output_interface, 1));
        wl_output_add_listener (output, &maliit_output_listener, this);
    }
}

void WindowedSurfaceFactoryPrivate::handleRegistryGlobalRemove(uint32_t name)
{
    Q_UNUSED(name);
}

void WindowedSurfaceFactoryPrivate::handleOutputGeometry(int32_t x,
                                                         int32_t y,
                                                         int32_t physical_width,
                                                         int32_t physical_height,
                                                         int32_t subpixel,
                                                         const char *make,
                                                         const char *model,
                                                         int32_t transform)
{
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(physical_width);
    Q_UNUSED(physical_height);
    Q_UNUSED(subpixel);
    Q_UNUSED(make);
    Q_UNUSED(model);
    Q_UNUSED(transform);
}

void WindowedSurfaceFactoryPrivate::handleOutputMode(uint32_t flags,
                                                     int32_t width,
                                                     int32_t height,
                                                     int32_t refresh)
{
    Q_UNUSED(width);
    Q_UNUSED(height);
    Q_UNUSED(refresh);
    qDebug() << __PRETTY_FUNCTION__;
    if ((flags & WL_OUTPUT_MODE_CURRENT) == WL_OUTPUT_MODE_CURRENT) {
        output_configured = true;
    }
}
#endif

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

#ifdef HAVE_WAYLAND
void *WindowedSurfaceFactory::getInputPanelSurface(QWindow *window)
{
    Q_D(WindowedSurfaceFactory);

    struct wl_surface *surface = static_cast<struct wl_surface *>(QGuiApplication::platformNativeInterface()->nativeResourceForWindow("surface", window));

    return input_panel_get_input_panel_surface(d->panel, surface);
}
#endif

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

namespace {

QSharedPointer<WindowedSurface> createSurface(WindowedSurfaceFactory *factory,
                                              AbstractSurface::Options options,
                                              const QSharedPointer<WindowedSurface> &parent)
{
    if (options & Maliit::Plugins::AbstractSurface::TypeGraphicsView) {
        return QSharedPointer<WindowedSurface>(new WindowedGraphicsViewSurface(factory,
                                                                               options,
                                                                               parent));
    }
    if (options & Maliit::Plugins::AbstractSurface::TypeWidget) {
        return QSharedPointer<WindowedSurface>(new WindowedWidgetSurface(factory,
                                                                         options,
                                                                         parent));
    }
    return QSharedPointer<WindowedSurface>();
}

} // unnamed namespace

QSharedPointer<AbstractSurface> WindowedSurfaceFactory::create(AbstractSurface::Options options,
                                                               const QSharedPointer<AbstractSurface> &parent)
{
    Q_D(WindowedSurfaceFactory);

    QSharedPointer<WindowedSurface> default_surface_parent(qSharedPointerDynamicCast<WindowedSurface>(parent));
    QSharedPointer<WindowedSurface> new_surface(createSurface(this, options,
                                                              default_surface_parent));

    if (new_surface) {
        d->surfaces.push_back(new_surface);
        Q_EMIT surfaceWidgetCreated(new_surface->widget(), options);
    }
    return new_surface;
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
