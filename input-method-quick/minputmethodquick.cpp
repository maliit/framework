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

    Q_DECLARE_PUBLIC(MInputMethodQuick);

    MInputMethodQuickPrivate(QWidget *mainWindow,
                             MInputMethodQuick *im)
        : q_ptr(im)
        , scene(new QGraphicsScene(computeDisplayRect(), im))
        , view(new MImGraphicsView(scene, mainWindow))
        , loader(new MInputMethodQuickLoader(scene, im))
    {}

    ~MInputMethodQuickPrivate()
    {
        delete loader;
        delete view;
        delete scene;
    }

    void handleRegionUpdate(MAbstractInputMethodHost *host,
                            const QRegion &region)
    {
        host->setScreenRegion(region);
    }

    void handleInputMethodAreaUpdate(MAbstractInputMethodHost *host,
                                     const QRegion &region)
    {
        host->setInputMethodArea(region);
    }

};

MInputMethodQuick::MInputMethodQuick(MAbstractInputMethodHost *host,
                                     QWidget *mainWindow,
                                     const QString &qmlFileName)
    : MAbstractInputMethod(host, mainWindow)
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

void MInputMethodQuick::show()
{
    Q_D(MInputMethodQuick);

    d->loader->showUI();
    const QRegion r(inputMethodArea());
    d->handleRegionUpdate(inputMethodHost(), r);
    d->handleInputMethodAreaUpdate(inputMethodHost(), r);
}

void MInputMethodQuick::hide()
{
    Q_D(MInputMethodQuick);

    d->loader->hideUI();
    const QRegion r;
    d->handleRegionUpdate(inputMethodHost(), r);
    d->handleInputMethodAreaUpdate(inputMethodHost(), r);
}

void MInputMethodQuick::setToolbar(QSharedPointer<const MToolbarData> toolbar)
{
    Q_D(MInputMethodQuick);

    d->loader->setToolbar(toolbar);
}

void MInputMethodQuick::propagateScreenSize()
{
    emit screenWidthChanged(computeDisplayRect().width());
    emit screenHeightChanged(computeDisplayRect().height());
}

int MInputMethodQuick::screenHeight() const
{
    return computeDisplayRect().height();
}

int MInputMethodQuick::screenWidth() const
{
    return computeDisplayRect().width();
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
        emit inputMethodAreaChanged(d->inputMethodArea);
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
    if (text == "\r\n") {
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
