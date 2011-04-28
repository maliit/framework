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

class MInputMethodQuickPrivate;
class MInputMethodQuickLoader;

//! \brief MInputMethodQuick is used for QML-based input method plugins.
//!
//! It defines the interface between framework, applications and QML-based
//! input methods. Instead of allowing QML-based input methods to use the
//! MAbstractInputMethodHost interface directly, this class will forward the
//! necessary requests.
//! QML-based input methods on the other hand can use the properties of this
//! class to respond to requests from the framework.
class MInputMethodQuick
    : public MAbstractInputMethod
{
    Q_OBJECT
    Q_ENUMS(KeyEvent)

    //! Propagates screen width to QML components.
    Q_PROPERTY(int screenWidth READ screenWidth
                               NOTIFY screenWidthChanged)

    //! Propagates screen height to QML components.
    Q_PROPERTY(int screenHeight READ screenHeight
                                NOTIFY screenHeightChanged)

public:
    enum KeyEvent { KeyPress = QEvent::KeyPress,
		    KeyRelease = QEvent::KeyRelease };

    //! Constructor
    //! \param host serves as communication link to framework and application. Managed by framework.
    //! \param mainWindow should be used to install plugin's UI into it. Managed by framework.
    //! \param qmlFileName the QML file that will be loaded.
    explicit MInputMethodQuick(MAbstractInputMethodHost *host,
                               QWidget *mainWindow,
                               const QString &qmlFileName);
    virtual ~MInputMethodQuick();

    //! \reimp
    virtual void show();
    virtual void hide();
    virtual void setToolbar(QSharedPointer<const MToolbarData> toolbar);
    //! \reimp_end

    //! Propagates screen size to QML components.
    void propagateScreenSize();

    //! Returns screen height.
    int screenHeight() const;

    //! Returns screen width.
    int screenWidth() const;

    //! Returns input method area.
    QRect inputMethodArea() const;

    //! Sets input method area. Called by QML components.
    //! area the area consumed by the QML input method.
    Q_INVOKABLE void setInputMethodArea(const QRect &area);

signals:
    //! Emitted when screen height changes.
    void screenHeightChanged(int height);

    //! Emitted when screen width changes.
    void screenWidthChanged(int width);

    //! Emitted when input method area changes.
    void inputMethodAreaChanged(const QRect &area);

public slots:
    //! Sends preedit string. Called by QML components.
    //! \param text the preedit string.
    void sendPreedit(const QString &text);

    //! Sends commit string. Called by QML components.
    //! \param text the commit string.
    void sendCommit(const QString &text);

    //! Tells the framework to switch plugins. Called by QML components.
    void pluginSwitchRequired(int switchDirection);

private:
    Q_DISABLE_COPY(MInputMethodQuick);
    Q_DECLARE_PRIVATE(MInputMethodQuick);
    MInputMethodQuickPrivate *const d_ptr;
};

#endif // MEEGO_KEYBOARD_QUICK_H
