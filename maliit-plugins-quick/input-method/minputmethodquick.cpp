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

#include "minputmethodquick.h"

#include "mkeyoverridequick.h"
#include "minputmethodquickplugin.h"

#include <maliit/plugins/abstractinputmethodhost.h>
#include <maliit/plugins/abstractsurfacefactory.h>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <maliit/plugins/quickviewsurface.h>
#include <QtQuick>
#include <QtGui>
#include <qpa/qplatformnativeinterface.h>
#include <xcb/xcb.h>
#include <xcb/xfixes.h>
#else
#include <maliit/plugins/abstractwidgetssurface.h>
#endif
#include <maliit/plugins/keyoverride.h>

#include <QKeyEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QRectF>
#include <QRect>
#include <QPainter>
#include <QPen>
#include <QBrush>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#else
#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#endif
#include <QGraphicsTextItem>
#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QDir>
#include <memory>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#if defined(Q_WS_X11)
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>
#endif
#endif

//this hack is needed because KeyPress definded by xlib conflicts with QEvent::KeyPress, breaking build
#undef KeyPress

namespace
{
    const char * const actionKeyName = "actionKey";

    const QRect &computeDisplayRect(QWidget *w = 0)
    {
        static const QRect displayRect(w ? qApp->desktop()->screenGeometry(w)
                                         : qApp->desktop()->screenGeometry());

        return displayRect;
    }
}

// TODO: Remove typedefs for Maliit 1.0 and only use the Qt5 types instead.
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
typedef QQmlEngine MaliitQmlEngine;
typedef QQmlComponent MaliitQmlComponent;
#else
typedef QDeclarativeEngine MaliitQmlEngine;
typedef QDeclarativeComponent MaliitQmlComponent;
#endif

//! Helper class to load QML files and set up the declarative view accordingly.
class MInputMethodQuickLoader
{
private:
    QGraphicsScene *const m_scene;
    MaliitQmlEngine *const m_engine; //!< managed by controller
    std::auto_ptr<MaliitQmlComponent> m_component;

    QGraphicsObject *m_content; //!< managed by scene
    MInputMethodQuick *const m_controller;

public:
    MInputMethodQuickLoader(QGraphicsScene *newScene,
                            MInputMethodQuick *newController)
        : m_scene(newScene)
        , m_engine(new MaliitQmlEngine(newController))
        , m_content(0)
        , m_controller(newController)
    {
        Q_ASSERT(m_scene);
        Q_ASSERT(m_controller);

        m_engine->rootContext()->setContextProperty("MInputMethodQuick", m_controller);
        m_engine->addImportPath(MALIIT_PLUGINS_DATA_DIR);

        Q_FOREACH (const QString &path, MInputMethodQuickPlugin::qmlImportPaths()) {
            m_engine->addImportPath(path);
        }

        // Assuming that plugin B loads after plugin A, we need to make sure
        // that plugin B does not use the customized import paths of plugin A:
        MInputMethodQuickPlugin::setQmlImportPaths(QStringList());
    }

    virtual ~MInputMethodQuickLoader()
    {}

    // TODO: rename to showContent?
    void showUI()
    {
        if(not m_content) {
            qWarning() << __PRETTY_FUNCTION__
                       << "Content or controller missing: Cannot show UI.";
            return;
        }

        m_controller->setActive(true);
    }

    void hideUI()
    {
        if(not m_content) {
            return;
        }

        m_controller->setActive(false);
    }

    void loadQmlFile(const QString &qmlFileName)
    {
        if (m_content) {
            qWarning() << "Qml file already loaded";
            return;
        }

        m_component.reset(new MaliitQmlComponent(m_engine, QUrl(qmlFileName)));

        if (not m_component->errors().isEmpty()) {
            qWarning() << "QML errors while loading " << qmlFileName << "\n"
                       << m_component->errors();
        }

        m_content = qobject_cast<QGraphicsObject *>(m_component->create());

        if (not m_content) {
            m_content = new QGraphicsTextItem("Error loading QML");
        }

        m_content->hide();
        m_scene->addItem(m_content);
    }

    QGraphicsObject *content() const
    {
        return m_content;
    }
};

class MInputMethodQuickPrivate
{
public:
    MInputMethodQuick *const q_ptr;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QSharedPointer<Maliit::Plugins::QuickViewSurface> surface;
#else
    QSharedPointer<Maliit::Plugins::AbstractGraphicsViewSurface> surface;
    QGraphicsScene *const scene;
    QGraphicsView *const view;
    MInputMethodQuickLoader *const loader;
#endif
    QRect inputMethodArea;
    int appOrientation;
    bool haveFocus;

    //! current active state
    Maliit::HandlerState activeState;

    //! In practice show() and hide() correspond to application SIP (close)
    //! requests.  We track the current shown/SIP requested state using these variables.
    bool sipRequested;
    bool sipIsInhibited;
    QSharedPointer<MKeyOverrideQuick> actionKeyOverride;
    QSharedPointer<MKeyOverride> sentActionKeyOverride;
    bool active;

    Q_DECLARE_PUBLIC(MInputMethodQuick)

    MInputMethodQuickPrivate(MAbstractInputMethodHost *host,
                             MInputMethodQuick *im)
        : q_ptr(im)
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        , surface(qSharedPointerDynamicCast<Maliit::Plugins::QuickViewSurface>(host->surfaceFactory()->create(Maliit::Plugins::AbstractSurface::PositionCenterBottom | Maliit::Plugins::AbstractSurface::TypeQuick2)))
#else
        , surface(qSharedPointerDynamicCast<Maliit::Plugins::AbstractGraphicsViewSurface>(host->surfaceFactory()->create(Maliit::Plugins::AbstractSurface::PositionCenterBottom | Maliit::Plugins::AbstractSurface::TypeGraphicsView)))
        , scene(surface->scene())
        , view(surface->view())
        , loader(new MInputMethodQuickLoader(scene, im))
#endif
        , appOrientation(0)
        , haveFocus(false)
        , activeState(Maliit::OnScreen)
        , sipRequested(false)
        , sipIsInhibited(false)
        , actionKeyOverride(new MKeyOverrideQuick())
        , sentActionKeyOverride()
        , active(false)
    {
        Q_ASSERT(surface);

        updateActionKey(MKeyOverride::All);
        // Set surface size to fullscreen
        surface->setSize(QApplication::desktop()->screenGeometry().size());
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        surface->view()->engine()->addImportPath(MALIIT_PLUGINS_DATA_DIR);
        surface->view()->engine()->rootContext()->setContextProperty("MInputMethodQuick", im);
#endif
    }

    ~MInputMethodQuickPrivate()
    {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        delete loader;
#endif
    }

    void handleInputMethodAreaUpdate(MAbstractInputMethodHost *host,
                                     const QRegion &region)
    {
        if (not host) {
            return;
        }

        host->setInputMethodArea(region);
    }

    void updateActionKey (const MKeyOverride::KeyOverrideAttributes changedAttributes)
    {
        actionKeyOverride->applyOverride(sentActionKeyOverride, changedAttributes);
    }
    
    void syncInputMask ()
    {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#if defined(Q_WS_X11)
        if (!view->effectiveWinId())
            return;

        const int size = 1;

        XRectangle * const rects = new XRectangle[size];

        quint32 customRegion[size * 4]; // custom region is pack of x, y, w, h
        rects[0].x = inputMethodArea.x();
        rects[0].y = inputMethodArea.y();
        rects[0].width = inputMethodArea.width();
        rects[0].height = inputMethodArea.height();
        customRegion[0] = inputMethodArea.x();
        customRegion[1] = inputMethodArea.y();
        customRegion[2] = inputMethodArea.width();
        customRegion[3] = inputMethodArea.height();


        const XserverRegion shapeRegion = XFixesCreateRegion(QX11Info::display(), rects, size);
        XFixesSetWindowShapeRegion(QX11Info::display(), view->effectiveWinId(), ShapeBounding, 0, 0, 0);
        XFixesSetWindowShapeRegion(QX11Info::display(), view->effectiveWinId(), ShapeInput, 0, 0, shapeRegion);

        XFixesDestroyRegion(QX11Info::display(), shapeRegion);

        XChangeProperty(QX11Info::display(), view->effectiveWinId(),
                        XInternAtom(QX11Info::display(), "_MEEGOTOUCH_CUSTOM_REGION", False),
                        XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char *) customRegion, size * 4);

        delete[] rects;
#endif
#else
        if (QGuiApplication::platformName() != "xcb")
            return;

        QPlatformNativeInterface *nativeInterface = QGuiApplication::platformNativeInterface();
        xcb_connection_t *connection = static_cast<xcb_connection_t *>(nativeInterface->nativeResourceForWindow("connection", surface->view()));

        xcb_rectangle_t rect;
        rect.x = inputMethodArea.x();
        rect.y = inputMethodArea.y();
        rect.width = inputMethodArea.width();
        rect.height = inputMethodArea.height();

        xcb_xfixes_region_t region = xcb_generate_id(connection);
        xcb_xfixes_create_region(connection, region, 1, &rect);

        xcb_window_t window  = surface->view()->winId();
        xcb_xfixes_set_window_shape_region(connection, window, XCB_SHAPE_SK_BOUNDING, 0, 0, 0);
        xcb_xfixes_set_window_shape_region(connection, window, XCB_SHAPE_SK_INPUT, 0, 0, region);

        xcb_xfixes_destroy_region(connection, region);
#endif
    }
};

MInputMethodQuick::MInputMethodQuick(MAbstractInputMethodHost *host,
                                     const QString &qmlFileName)
    : MAbstractInputMethod(host)
    , d_ptr(new MInputMethodQuickPrivate(host, this))
{
    Q_D(MInputMethodQuick);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    d->surface->view()->setSource(QUrl::fromLocalFile(qmlFileName));
#else
    d->loader->loadQmlFile(qmlFileName);
#endif
    
    propagateScreenSize();
}

MInputMethodQuick::~MInputMethodQuick()
{}

void MInputMethodQuick::handleFocusChange(bool focusIn)
{
    Q_D(MInputMethodQuick);
    d->haveFocus = focusIn;
    Q_EMIT focusTargetChanged(focusIn);
}

void MInputMethodQuick::show()
{
    Q_D(MInputMethodQuick);
    d->sipRequested = true;
    if (d->sipIsInhibited) {
        return;
    }

    handleAppOrientationChanged(d->appOrientation);
    
    if (d->activeState == Maliit::OnScreen) {
      d->surface->show();
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
      d->loader->showUI();
#else
      setActive(true);
#endif
      d->syncInputMask();
    }
}

void MInputMethodQuick::hide()
{
    Q_D(MInputMethodQuick);
    if (!d->sipRequested) {
        return;
    }
    d->sipRequested = false;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    d->loader->hideUI();
#else
    setActive(false);
#endif
    d->surface->hide();
    const QRegion r;
    d->handleInputMethodAreaUpdate(inputMethodHost(), r);
}

void MInputMethodQuick::handleAppOrientationChanged(int angle)
{
    Q_D(MInputMethodQuick);

    MAbstractInputMethod::handleAppOrientationChanged(angle);

    if (d->appOrientation != angle) {

        d->appOrientation = angle;
        // When emitted, QML Plugin will realice a state
        // change and update InputMethodArea. Don't propagate those changes except if
        // VkB is currently showed
        Q_EMIT appOrientationChanged(d->appOrientation);
        if (d->sipRequested && !d->sipIsInhibited) {
            d->handleInputMethodAreaUpdate(inputMethodHost(), inputMethodArea().toRect());
        }
    }
}

void MInputMethodQuick::setState(const QSet<Maliit::HandlerState> &state)
{
    Q_D(MInputMethodQuick);

    if (state.isEmpty()) {
        return;
    }

    if (state.contains(Maliit::OnScreen)) {
        d->activeState = Maliit::OnScreen;
        if (d->sipRequested && !d->sipIsInhibited) {
            show(); // Force reparent of client widgets.
        }
    } else {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        d->loader->hideUI();
#else
        setActive(false);
#endif
        // Allow client to make use of InputMethodArea
        const QRegion r;
        d->handleInputMethodAreaUpdate(inputMethodHost(), r);
        d->activeState = *state.begin();
    }
}

void MInputMethodQuick::handleClientChange()
{
    Q_D(MInputMethodQuick);

    if (d->sipRequested) {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        d->loader->hideUI();
#else
        setActive(false);
#endif
    }
}

void MInputMethodQuick::handleVisualizationPriorityChange(bool inhibitShow)
{
    Q_D(MInputMethodQuick);

    if (d->sipIsInhibited == inhibitShow) {
        return;
    }
    d->sipIsInhibited = inhibitShow;

    if (d->sipRequested) {
        if (inhibitShow) {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
            d->loader->hideUI();
#else
            setActive(false);
#endif
        } else {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
            d->loader->showUI();
#else
            setActive(true);
#endif
        }
    }
}


void MInputMethodQuick::propagateScreenSize()
{
    Q_EMIT screenWidthChanged(computeDisplayRect().width());
    Q_EMIT screenHeightChanged(computeDisplayRect().height());
}

int MInputMethodQuick::screenHeight() const
{
    return computeDisplayRect().height();
}

int MInputMethodQuick::screenWidth() const
{
    return computeDisplayRect().width();
}

int MInputMethodQuick::appOrientation() const
{
    Q_D(const MInputMethodQuick);
    return d->appOrientation;
}

QRectF MInputMethodQuick::inputMethodArea() const
{
    Q_D(const MInputMethodQuick);
    return d->inputMethodArea;
}

void MInputMethodQuick::setInputMethodArea(const QRectF &area)
{
    Q_D(MInputMethodQuick);

    if (d->inputMethodArea != area.toRect()) {
        d->inputMethodArea = area.toRect();
        d->handleInputMethodAreaUpdate(inputMethodHost(), d->inputMethodArea);

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        qDebug() << __PRETTY_FUNCTION__ << "QWidget::effectiveWinId(): " << d_ptr->view->effectiveWinId();
#endif

        Q_EMIT inputMethodAreaChanged(d->inputMethodArea);
        d->syncInputMask();
    }
}

void MInputMethodQuick::setScreenRegion(const QRect &region)
{
    inputMethodHost()->setScreenRegion(region);
}

void MInputMethodQuick::sendPreedit(const QString &text)
{
    QList<Maliit::PreeditTextFormat> lst;
    inputMethodHost()->sendPreeditString(text, lst, text.length());
}

void MInputMethodQuick::sendKey(int key, int modifiers, const QString &text)
{
    QKeyEvent event(QEvent::KeyPress, key, (~(Qt::KeyboardModifiers(Qt::NoModifier))) & modifiers, text);
    inputMethodHost()->sendKeyEvent(event);
}

void MInputMethodQuick::sendCommit(const QString &text)
{
    if (text == "\b") {
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
        inputMethodHost()->sendKeyEvent(event);
    } else
    if ((text == "\r\n") || (text == "\n") || (text == "\r")) {
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
        inputMethodHost()->sendKeyEvent(event);
    } else {
        inputMethodHost()->sendCommitString(text);
    }
}

void MInputMethodQuick::pluginSwitchRequired(int switchDirection)
{
    inputMethodHost()->switchPlugin(
        static_cast<Maliit::SwitchDirection>(switchDirection));
}

void MInputMethodQuick::userHide()
{
    hide();
    inputMethodHost()->notifyImInitiatedHiding();
}

void MInputMethodQuick::setKeyOverrides(const QMap<QString, QSharedPointer<MKeyOverride> > &overrides)
{
    Q_D(MInputMethodQuick);
    const QMap<QString, QSharedPointer<MKeyOverride> >::const_iterator iter(overrides.find(actionKeyName));

    if (d->sentActionKeyOverride) {
        disconnect(d->sentActionKeyOverride.data(), SIGNAL(keyAttributesChanged(const QString &, const MKeyOverride::KeyOverrideAttributes)),
                   this, SLOT(onSentActionKeyAttributesChanged(const QString &, const MKeyOverride::KeyOverrideAttributes)));
        d->sentActionKeyOverride.clear();
    }

    if (iter != overrides.end()) {
        QSharedPointer<MKeyOverride> sentActionKeyOverride(*iter);

        if (sentActionKeyOverride) {
            d->sentActionKeyOverride = sentActionKeyOverride;
            connect(d->sentActionKeyOverride.data(), SIGNAL(keyAttributesChanged(const QString &, const MKeyOverride::KeyOverrideAttributes)),
                    this, SLOT(onSentActionKeyAttributesChanged(const QString &, const MKeyOverride::KeyOverrideAttributes)));
        }
    }
    d->updateActionKey(MKeyOverride::All);
}

QList<MAbstractInputMethod::MInputMethodSubView> MInputMethodQuick::subViews(Maliit::HandlerState state) const
{
    Q_UNUSED(state);
    MAbstractInputMethod::MInputMethodSubView sub_view;
    sub_view.subViewId = "";
    sub_view.subViewTitle = "";
    QList<MAbstractInputMethod::MInputMethodSubView> sub_views;
    sub_views << sub_view;
    return sub_views;
}

void MInputMethodQuick::onSentActionKeyAttributesChanged(const QString &, const MKeyOverride::KeyOverrideAttributes changedAttributes)
{
    Q_D(MInputMethodQuick);

    d->updateActionKey(changedAttributes);
}

MKeyOverrideQuick* MInputMethodQuick::actionKeyOverride() const
{
    Q_D(const MInputMethodQuick);

    return d->actionKeyOverride.data();
}

void MInputMethodQuick::activateActionKey()
{
    sendCommit("\n");
}

bool MInputMethodQuick::isActive() const
{
    Q_D(const MInputMethodQuick);
    return d->active;
}

void MInputMethodQuick::setActive(bool enable)
{
    Q_D(MInputMethodQuick);
    if (d->active != enable) {
        d->active = enable;
        Q_EMIT activeChanged();
    }
}
