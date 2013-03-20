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

#ifndef MALIIT_INPUT_METHOD_QUICK_H
#define MALIIT_INPUT_METHOD_QUICK_H

#include <maliit/plugins/abstractinputmethod.h>
#include <maliit/plugins/keyoverride.h>
#include "maliitquick.h"

#include <QEvent>
#include <QPointer>
#include <QRect>
#include <QScopedPointer>
#include <QString>

namespace Maliit
{

class AbstractPlatform;
class InputMethodQuickPrivate;
class KeyOverrideQuick;

//! \brief Maliit::InputMethodQuick is used for QML-based input method plugins.
//!
//! It defines the interface between framework, applications and QML-based input
//! methods. Instead of allowing QML-based input methods to use the
//! MAbstractInputMethodHost interface directly, this class will forward the
//! necessary requests.
//! QML-based input methods on the other hand can use the properties of this
//! class to respond to requests from the framework.
class InputMethodQuick
    : public MAbstractInputMethod
{
    Q_OBJECT

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
    Q_PROPERTY(Maliit::KeyOverrideQuick *actionKeyOverride READ actionKeyOverride
                                                           NOTIFY actionKeyOverrideChanged)

    //! Property for whether input method is active
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)

    Q_PROPERTY(bool surroundingTextValid READ surroundingTextValid NOTIFY surroundingTextValidChanged)
    Q_PROPERTY(QString surroundingText READ surroundingText NOTIFY surroundingTextChanged)
    Q_PROPERTY(int cursorPosition READ cursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(int anchorPosition READ anchorPosition NOTIFY anchorPositionChanged)
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY hasSelectionChanged)
    Q_PROPERTY(int contentType READ contentType NOTIFY contentTypeChanged)
    Q_PROPERTY(bool predictionEnabled READ predictionEnabled NOTIFY predictionEnabledChanged)
    Q_PROPERTY(bool autoCapitalizationEnabled READ autoCapitalizationEnabled NOTIFY autoCapitalizationChanged)
    Q_PROPERTY(bool hiddenText READ hiddenText NOTIFY hiddenTextChanged)

public:
    //! Constructor
    //! \param host serves as communication link to framework and application. Managed by framework.
    //! \param qmlFileName the QML file that will be loaded.
    //! \param platform implementation used for e.g. setting input panel region.
    explicit InputMethodQuick(MAbstractInputMethodHost *host,
                              const QString &qmlFileName,
                              const QSharedPointer<Maliit::AbstractPlatform> &platform);
    virtual ~InputMethodQuick();

    //! \reimp
    virtual void show();
    virtual void hide();
    virtual void update();
    virtual void reset();
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
    KeyOverrideQuick *actionKeyOverride() const;

    //! Activates action key, that is - sends enter keypress.
    Q_INVOKABLE void activateActionKey();

    //! Return true on input method expected to be shown
    bool isActive() const;
    //! Sets input method expected to be shown/hidden
    void setActive(bool enable);

    bool surroundingTextValid();
    QString surroundingText();
    int cursorPosition();
    int anchorPosition();
    bool hasSelection();
    int contentType();
    bool predictionEnabled();
    bool autoCapitalizationEnabled();
    bool hiddenText();

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

    //! Emitted when focus target changes. activeEditor is true if there's an active editor afterwards.
    void focusTargetChanged(bool activeEditor);

    //! Emitted when input method state was reset from application side.
    void inputMethodReset();

    //! Emitted last when editor state has updated. In addition change signals are emitted for distinct property changes.
    void editorStateUpdate();

    void surroundingTextValidChanged();
    void surroundingTextChanged();
    void cursorPositionChanged();
    void anchorPositionChanged();
    void hasSelectionChanged();
    void contentTypeChanged();
    void predictionEnabledChanged();
    void autoCapitalizationChanged();
    void hiddenTextChanged();

public Q_SLOTS:
    //! Sends preedit string. Called by QML components. See also MAbstractInputMethodHost::sendPreeditString()
    //! \param text the preedit string.
    //! \param preeditFormats Selects visual stylings for each part of preedit. The value can be either:
    //! Maliit.PreeditFace for applying one style for whole string or list of lists containing [PreeditFace, start, length]
    //! \param replacementStart The position at which characters are to be replaced relative
    //! from the start of the preedit string.
    //! \param replacementLength The number of characters to be replaced in the preedit string.
    //! \param cursorPos The cursur position inside preedit
    void sendPreedit(const QString &text, const QVariant &preeditFormats = QVariant(),
                     int replacementStart = 0, int replacementLength = 0, int cursorPos = -1);

    //! Sends an arbitrary key, optionally with modifiers.
    //! \param key the Qt keycode to be sent, e.g., Qt.Key_Up.
    //! \param modifiers optional modifiers to send along, like Qt.ControlModifier.
    //! \param text an optional text to send along with the QKeyEvent.
    //! \param type MaliitQuick.KeyPress, KeyRelease or KeyClick for both
    void sendKey(int key, int modifiers = 0, const QString &text = QString(), int type = MaliitQuick::KeyClick);

    //! Sends commit string. Called by QML components. For params, see also MAbstractInputMethodHost::sendCommitString()
    //! \param text the commit string.
    //! \param replaceStart The position at which characters are to be replaced relative to the start of the
    //! preedit string.
    //! \param replaceLength The number of characters to be replaced in the preedit string.
    //! \param cursorPos The cursor position to be set, relative to commit string start.
    //!  Negative values are used as commit string end position.
    void sendCommit(const QString &text, int replaceStart = 0, int replaceLength = 0, int cursorPos = -1);

    //! Tells the framework to switch plugins. Called by QML components.
    void pluginSwitchRequired(int switchDirection);

    //! Tells the framework to close keyboard. Called by QML components.
    void userHide();

private:
    Q_DISABLE_COPY(InputMethodQuick)
    Q_DECLARE_PRIVATE(InputMethodQuick)

    const QScopedPointer<InputMethodQuickPrivate> d_ptr;

private Q_SLOTS:
    //! Propagates change to QML.
    void onSentActionKeyAttributesChanged(const QString &keyId, const MKeyOverride::KeyOverrideAttributes changedAttributes);
};

} // namespace Maliit

#endif // MALIIT_INPUT_METHOD_QUICK_H
