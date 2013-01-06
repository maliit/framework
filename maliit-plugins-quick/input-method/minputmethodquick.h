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

#ifndef MEEGO_KEYBOARD_QUICK_H
#define MEEGO_KEYBOARD_QUICK_H

#include <maliit/plugins/abstractinputmethod.h>
#include <maliit/plugins/keyoverride.h>

#include <QRect>
#include <QPointer>
#include <QString>
#include <QEvent>
#include <QGraphicsView>
#include <QGraphicsScene>

class MInputMethodQuickPrivate;
class MInputMethodQuickLoader;
class MKeyOverrideQuick;

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

    //! Propagates application orientation to QML components.
    Q_PROPERTY(int appOrientation READ appOrientation
                                  NOTIFY appOrientationChanged)

    //! Propagates action key override to QML components.
    Q_PROPERTY(MKeyOverrideQuick *actionKeyOverride READ actionKeyOverride
                                                    NOTIFY actionKeyOverrideChanged)

    //! Property for whether input method is active
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)

public:
    enum KeyEvent { KeyPress = QEvent::KeyPress,
                    KeyRelease = QEvent::KeyRelease };

    //! Constructor
    //! \param host serves as communication link to framework and application. Managed by framework.
    //! \param mainWindow should be used to install plugin's UI into it. Managed by framework.
    //! \param qmlFileName the QML file that will be loaded.
    explicit MInputMethodQuick(MAbstractInputMethodHost *host,
                               const QString &qmlFileName);
    virtual ~MInputMethodQuick();

    //! \reimp
    virtual void show();
    virtual void hide();
    virtual void handleVisualizationPriorityChange(bool priority);
    virtual void handleClientChange();
    virtual void handleAppOrientationChanged(int angle);
    virtual void setState(const QSet<Maliit::HandlerState> &state);

    virtual void setKeyOverrides(const QMap<QString, QSharedPointer<MKeyOverride> > &overrides);
    virtual void handleFocusChange(bool focusIn);
    QList<MAbstractInputMethod::MInputMethodSubView> subViews(Maliit::HandlerState state) const;
    //! \reimp_end

    //! Propagates screen size to QML components.
    void propagateScreenSize();

    //! Returns screen height.
    int screenHeight() const;

    //! Returns screen width.
    int screenWidth() const;

    //! Returns application orientation.
    int appOrientation() const;

    //! Returns input method area.
    QRectF inputMethodArea() const;

    //! Sets input method area. Called by QML components.
    //! area the area consumed by the QML input method. On transitions can reserve target area at start.
    Q_INVOKABLE void setInputMethodArea(const QRectF &area);

    //! Sets area input method is actually using from the screen.
    Q_INVOKABLE void setScreenRegion(const QRect &region);

    //! Returns action key override.
    MKeyOverrideQuick *actionKeyOverride() const;

    //! Activates action key, that is - sends enter keypress.
    Q_INVOKABLE void activateActionKey();

    //! Return true on input method expected to be shown
    bool isActive() const;
    //! Sets input method expected to be shown/hidden
    void setActive(bool enable);

Q_SIGNALS:
    //! Emitted when screen height changes.
    void screenHeightChanged(int height);

    //! Emitted when screen width changes.
    void screenWidthChanged(int width);

    //! Emitted when application orientation changes.
    void appOrientationChanged(int angle);

    //! Emitted when input method area changes.
    void inputMethodAreaChanged(const QRect &area);

    //! Emitted when key action override changes.
    void actionKeyOverrideChanged(MKeyOverride *override);

    void activeChanged();

public Q_SLOTS:
    //! Sends preedit string. Called by QML components.
    //! \param text the preedit string.
    void sendPreedit(const QString &text);

    //! Sends an arbitrary key, optionally with modifiers.
    //! \param key the Qt keycode to be sent, e.g., Qt.Key_Up.
    //! \param modifiers optional modifiers to send along, like Qt.ControlModifier.
    //! \param text an optional text to send along with the QKeyEvent.
    void sendKey(int key, int modifiers = 0, const QString &text = QString());

    //! Sends commit string. Called by QML components.
    //! \param text the commit string.
    void sendCommit(const QString &text);

    //! Tells the framework to switch plugins. Called by QML components.
    void pluginSwitchRequired(int switchDirection);

    //! Tells the framework to close keyboard. Called by QML components.
    void userHide();

private:
    Q_DISABLE_COPY(MInputMethodQuick)
    Q_DECLARE_PRIVATE(MInputMethodQuick)

    MInputMethodQuickPrivate *const d_ptr;

private Q_SLOTS:
    //! Propagates change to QML.
    void onSentActionKeyAttributesChanged(const QString &keyId, const MKeyOverride::KeyOverrideAttributes changedAttributes);
};

#endif // MEEGO_KEYBOARD_QUICK_H
