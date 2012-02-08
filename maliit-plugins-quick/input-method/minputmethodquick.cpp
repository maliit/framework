/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: Kimmo Surakka <kimmo.surakka@nokia.com>
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

#include "minputmethodquick.h"

#include "mabstractinputmethodhost.h"
#include "mimgraphicsview.h"
#include "mtoolbardata.h"
#include "mtoolbarlayout.h"
#include "mtoolbaritem.h"
#include "mkeyoverridequick.h"
#include "mkeyoverride.h"
#include "minputmethodquickplugin.h"

#include <QKeyEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QRectF>
#include <QRect>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QGraphicsTextItem>
#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QDir>
#include <memory>

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

//! Helper class to load QML files and set up the declarative view accordingly.
class MInputMethodQuickLoader
{
private:
    QGraphicsScene *const m_scene;
    QDeclarativeEngine *const m_engine; //!< managed by controller
    std::auto_ptr<QDeclarativeComponent> m_component;
    QGraphicsObject *m_content; //!< managed by scene
    MInputMethodQuick *const m_controller;

public:
    MInputMethodQuickLoader(QGraphicsScene *newScene,
                            MInputMethodQuick *newController)
        : m_scene(newScene)
        , m_engine(new QDeclarativeEngine(newController))
        , m_content(0)
        , m_controller(newController)
    {
        Q_ASSERT(m_scene);
        Q_ASSERT(m_controller);

        m_engine->rootContext()->setContextProperty("MInputMethodQuick", m_controller);
        m_engine->addImportPath(M_IM_PLUGINS_DATA_DIR);

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

        m_content->show();
    }

    void hideUI()
    {
        if(not m_content) {
            return;
        }

        m_content->hide();
    }

    void setToolbar(const QSharedPointer<const MToolbarData>&)
    {
        // Not implemented yet.
    }

    void loadQmlFile(const QString &qmlFileName)
    {
        const bool wasContentVisible(m_content ? m_content->isVisible() : false);

        if (wasContentVisible) {
            m_controller->hide();
        }

        m_component.reset(new QDeclarativeComponent(m_engine, QUrl(qmlFileName)));

        if (not m_component->errors().isEmpty()) {
            qWarning() << "QML errors while loading " << qmlFileName << "\n"
                       << m_component->errors();
        }

        m_content = qobject_cast<QGraphicsObject *>(m_component->create());

        if (not m_content) {
            m_content = new QGraphicsTextItem("Error loading QML");
        }

        if (wasContentVisible) {
            m_controller->show();
            m_content->show();
        } else {
            m_content->hide();
        }

        m_scene->addItem(m_content);
    }
};

class MInputMethodQuickPrivate
{
public:
    MInputMethodQuick *const q_ptr;
    QGraphicsScene *const scene;
    QGraphicsView *const view;
    MInputMethodQuickLoader *const loader;
    QRect inputMethodArea;
    int appOrientation;
    bool haveFocus;

    //! current active state
    MInputMethod::HandlerState activeState;

    //! In practice show() and hide() correspond to application SIP (close)
    //! requests.  We track the current shown/SIP requested state using these variables.
    bool sipRequested;
    bool sipIsInhibited;
    QSharedPointer<MKeyOverrideQuick> actionKeyOverride;
    QSharedPointer<MKeyOverride> sentActionKeyOverride;

    Q_DECLARE_PUBLIC(MInputMethodQuick);

    MInputMethodQuickPrivate(QWidget *mainWindow,
                             MInputMethodQuick *im)
        : q_ptr(im)
        , scene(new QGraphicsScene(computeDisplayRect(), im))
        , view(new MImGraphicsView(scene, mainWindow))
        , loader(new MInputMethodQuickLoader(scene, im))
        , appOrientation(0)
        , haveFocus(false)
        , activeState(MInputMethod::OnScreen)
        , sipRequested(false)
        , sipIsInhibited(false)
        , actionKeyOverride(new MKeyOverrideQuick())
        , sentActionKeyOverride()
    {
        updateActionKey(MKeyOverride::All);
    }

    ~MInputMethodQuickPrivate()
    {
        delete loader;
        delete view;
        delete scene;
    }

    void handleInputMethodAreaUpdate(MAbstractInputMethodHost *host,
                                     const QRegion &region)
    {
        if (not host) {
            return;
        }
        host->setScreenRegion(region);
        host->setInputMethodArea(region);
    }

    void updateActionKey (const MKeyOverride::KeyOverrideAttributes changedAttributes)
    {
        actionKeyOverride->applyOverride(sentActionKeyOverride, changedAttributes);
    }
};

MInputMethodQuick::MInputMethodQuick(MAbstractInputMethodHost *host,
                                     QWidget *mainWindow,
                                     const QString &qmlFileName)
    : MAbstractInputMethod(host)
    , d_ptr(new MInputMethodQuickPrivate(mainWindow, this))
{
    Q_D(MInputMethodQuick);

    d->loader->loadQmlFile(qmlFileName);
    propagateScreenSize();
    QWidget *p = d->view->viewport();

    // make sure the window gets displayed:
    if (p->nativeParentWidget()) {
        p = p->nativeParentWidget();
    }

    // TODO: Make it work on multi-display setups.
    // Would need to correctly follow current display of app window
    // (record last mouse event and get display through event's position?).
    const QRect displayRect(computeDisplayRect(p));
    d->view->resize(displayRect.size());
    d->view->setSceneRect(displayRect);
    d->view->show();
    d->view->setFrameShape(QFrame::NoFrame);
    d->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

MInputMethodQuick::~MInputMethodQuick()
{}

void MInputMethodQuick::handleFocusChange(bool focusIn)
{
    Q_D(MInputMethodQuick);
    d->haveFocus = focusIn;
}

void MInputMethodQuick::show()
{
    Q_D(MInputMethodQuick);
    d->sipRequested = true;
    if(d->sipIsInhibited) {
      return;
    }

    handleAppOrientationChanged(d->appOrientation);
    
    if (d->activeState == MInputMethod::OnScreen) {
      d->loader->showUI();
      const QRegion r(inputMethodArea());
      d->handleInputMethodAreaUpdate(inputMethodHost(), r);
    }
}

void MInputMethodQuick::hide()
{
    Q_D(MInputMethodQuick);
    if (!d->sipRequested) {
        return;
    }
    d->sipRequested = false;
    d->loader->hideUI();
    const QRegion r;
    d->handleInputMethodAreaUpdate(inputMethodHost(), r);
}

void MInputMethodQuick::setToolbar(QSharedPointer<const MToolbarData> toolbar)
{
    Q_D(MInputMethodQuick);

    d->loader->setToolbar(toolbar);
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
            d->handleInputMethodAreaUpdate(inputMethodHost(), inputMethodArea());
        }
    }
}

void MInputMethodQuick::setState(const QSet<MInputMethod::HandlerState> &state)
{
    Q_D(MInputMethodQuick);

    if (state.isEmpty()) {
        return;
    }

    if (state.contains(MInputMethod::OnScreen)) {
        d->activeState = MInputMethod::OnScreen;
        if (d->sipRequested && !d->sipIsInhibited) {
            show(); // Force reparent of client widgets.
        }
    } else {
        d->loader->hideUI();
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
        d->loader->hideUI();
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
            d->loader->hideUI();
        } else {
            d->loader->showUI();
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

QRect MInputMethodQuick::inputMethodArea() const
{
    Q_D(const MInputMethodQuick);
    return d->inputMethodArea;
}

void MInputMethodQuick::setInputMethodArea(const QRect &area)
{
    Q_D(MInputMethodQuick);

    if (d->inputMethodArea != area) {
        d->inputMethodArea = area;

        Q_EMIT inputMethodAreaChanged(d->inputMethodArea);
    }
}

void MInputMethodQuick::sendPreedit(const QString &text)
{
    QList<MInputMethod::PreeditTextFormat> lst;
    inputMethodHost()->sendPreeditString(text, lst, text.length());
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
        static_cast<MInputMethod::SwitchDirection>(switchDirection));
}

void MInputMethodQuick::userHide()
{
    hide();
    inputMethodHost()->notifyImInitiatedHiding();
}

void MInputMethodQuick::setKeyOverrides(const QMap<QString, QSharedPointer<MKeyOverride> > &overrides)
{
    Q_D(MInputMethodQuick);
    QMap<QString, QSharedPointer<MKeyOverride> >::iterator iter(overrides.find(actionKeyName));

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
