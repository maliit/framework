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

#ifndef MEEGO_KEYBOARD_QUICK_H
#define MEEGO_KEYBOARD_QUICK_H

#include <mabstractinputmethod.h>

#include <QRect>
#include <QPointer>
#include <QString>
#include <QEvent>
#include <QGraphicsView>
#include <QGraphicsScene>

class MInputMethodQuickLoader;

class MInputMethodQuick
    : public MAbstractInputMethod
{
    Q_OBJECT
    Q_ENUMS(KeyEvent)

    Q_PROPERTY(int screenWidth READ screenWidth
                               NOTIFY screenWidthChanged)

    Q_PROPERTY(int screenHeight READ screenHeight
                                NOTIFY screenHeightChanged)

public:
    enum KeyEvent { KeyPress = QEvent::KeyPress,
		    KeyRelease = QEvent::KeyRelease };

    explicit MInputMethodQuick(MAbstractInputMethodHost *host,
                               QWidget *mainWindow,
                               const QString &qmlFileName);
    virtual ~MInputMethodQuick();
    //! \reimp
    virtual void show();
    virtual void hide();
    virtual void setToolbar(QSharedPointer<const MToolbarData> toolbar);
    //! \reimp_end

    void propagateScreenSize();

    int screenHeight() const;
    int screenWidth() const;
    QRect inputMethodArea() const;

    Q_INVOKABLE void setInputMethodArea(const QRect &area);

signals:
    void screenHeightChanged();
    void screenWidthChanged();
    void inputMethodAreaChanged();

public slots:
    void preedit(const QString &text);
    void commit(const QString &text);
    void pluginSwitchRequired(int switchDirection);

private slots:
    void handleRegionUpdate(const QRegion &region);
    void handleInputMethodAreaUpdate(const QRegion &region);

private:
    // TODO: pimplify (exported class)
    MInputMethodQuick(const MInputMethodQuick &other);
    MInputMethodQuick& operator=(const MInputMethodQuick &other);

    MAbstractInputMethodHost *const m_host;
    QGraphicsScene *const m_scene;
    QGraphicsView *const m_view;
    MInputMethodQuickLoader *const m_loader;
};

#endif // MEEGO_KEYBOARD_QUICK_H
