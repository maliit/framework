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
#include "maliitquick.h"

#include <maliit/plugins/abstractinputmethodhost.h>
#include <maliit/plugins/keyoverride.h>

#include <QtCore>
#include <QtGui>

#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>

#include <qpa/qplatformnativeinterface.h>
#include <xcb/xcb.h>
#include <xcb/xfixes.h>

#ifdef HAVE_WAYLAND
#include <wayland-client.h>
#endif

namespace
{

const char * const actionKeyName = "actionKey";

QQuickView *getSurface (MAbstractInputMethodHost *host)
{
    QScopedPointer<QQuickView> view(new QQuickView (0));

    host->registerWindow (view.data(), Maliit::PositionCenterBottom);

    QSurfaceFormat format;
    format.setAlphaBufferSize(8);
    view->setFormat(format);
    view->setColor(QColor(Qt::transparent));

    return view.take ();
}

}

class MInputMethodQuickPrivate
{
    Q_DECLARE_PUBLIC(MInputMethodQuick)

public:
    MInputMethodQuick *const q_ptr;
    QScopedPointer<QQuickView> surface;
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

    bool m_surroundingTextValid;
    QString m_surroundingText;
    int m_cursorPosition;
    int m_anchorPosition;
    bool m_hasSelection;
    int m_contentType;
    bool m_predictionEnabled;
    bool m_autoCapitalizationEnabled;
    bool m_hiddenText;

    MInputMethodQuickPrivate(MAbstractInputMethodHost *host,
                             MInputMethodQuick *im)
        : q_ptr(im)
        , surface(getSurface(host))
        , appOrientation(0)
        , haveFocus(false)
        , activeState(Maliit::OnScreen)
        , sipRequested(false)
        , sipIsInhibited(false)
        , actionKeyOverride(new MKeyOverrideQuick())
        , sentActionKeyOverride()
        , active(false)
        , m_surroundingTextValid(false)
        , m_cursorPosition(-1)
        , m_anchorPosition(-1)
        , m_hasSelection(false)
        , m_contentType(MaliitQuick::FreeTextContentType)
        , m_predictionEnabled(true)
        , m_autoCapitalizationEnabled(true)
        , m_hiddenText(false)
    {
        Q_ASSERT(surface);

        updateActionKey(MKeyOverride::All);
        surface->engine()->addImportPath(MALIIT_PLUGINS_DATA_DIR);
        surface->engine()->rootContext()->setContextProperty("MInputMethodQuick", im);
    }

    ~MInputMethodQuickPrivate()
    {
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
        if (QGuiApplication::platformName() == "xcb") {
            QPlatformNativeInterface *nativeInterface = QGuiApplication::platformNativeInterface();
            xcb_connection_t *connection = static_cast<xcb_connection_t *>(nativeInterface->nativeResourceForWindow("connection", surface.data()));

            xcb_rectangle_t rect;
            rect.x = inputMethodArea.x();
            rect.y = inputMethodArea.y();
            rect.width = inputMethodArea.width();
            rect.height = inputMethodArea.height();

            xcb_xfixes_region_t region = xcb_generate_id(connection);
            xcb_xfixes_create_region(connection, region, 1, &rect);

            xcb_window_t window  = surface->winId();
            xcb_xfixes_set_window_shape_region(connection, window, XCB_SHAPE_SK_BOUNDING, 0, 0, 0);
            xcb_xfixes_set_window_shape_region(connection, window, XCB_SHAPE_SK_INPUT, 0, 0, region);

            xcb_xfixes_destroy_region(connection, region);
#ifdef HAVE_WAYLAND
        } else if (QGuiApplication::platformName() == "wayland"){
            QPlatformNativeInterface *nativeInterface = QGuiApplication::platformNativeInterface();

            wl_compositor *compositor = static_cast<wl_compositor *>(nativeInterface->nativeResourceForIntegration("compositor"));

            wl_region *region = wl_compositor_create_region(compositor);
            wl_region_add(region, inputMethodArea.x(), inputMethodArea.y(),
                          inputMethodArea.width(), inputMethodArea.height());

            wl_surface *wlsurface = static_cast<wl_surface *>(nativeInterface->nativeResourceForWindow("surface", surface.data()));
            wl_surface_set_input_region(wlsurface, region);
            wl_region_destroy(region);
#endif
        }
    }
};

MInputMethodQuick::MInputMethodQuick(MAbstractInputMethodHost *host,
                                     const QString &qmlFileName)
    : MAbstractInputMethod(host)
    , d_ptr(new MInputMethodQuickPrivate(host, this))
{
    Q_D(MInputMethodQuick);

    d->surface->setSource(QUrl::fromLocalFile(qmlFileName));
    
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
      d->surface->setGeometry(QRect(QPoint(), QGuiApplication::primaryScreen()->availableSize()));
      d->surface->show();
      setActive(true);
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
    setActive(false);
    d->surface->hide();
    const QRegion r;
    d->handleInputMethodAreaUpdate(inputMethodHost(), r);
}

void MInputMethodQuick::update()
{
    Q_D(MInputMethodQuick);

    bool emitSurroundingText = false;
    bool emitSurroundingTextValid = false;
    bool emitCursorPosition = false;
    bool emitAnchorPosition = false;
    bool emitSelection = false;
    bool emitContentType = false;
    bool emitAutoCapitalization = false;
    bool emitPredictionEnabled = false;
    bool emitHiddenText = false;

    QString newSurroundingText;
    int newCursorPosition = -1;
    inputMethodHost()->surroundingText(newSurroundingText, newCursorPosition);

    if (newSurroundingText != d->m_surroundingText) {
        d->m_surroundingText = newSurroundingText;
        emitSurroundingText = true;
    }

    bool newSurroundingTextValid = !newSurroundingText.isNull();
    if (newSurroundingTextValid != d->m_surroundingTextValid) {
        d->m_surroundingTextValid = newSurroundingTextValid;
        emitSurroundingTextValid = true;
    }

    if (newCursorPosition != d->m_cursorPosition) {
        d->m_cursorPosition = newCursorPosition;
        emitCursorPosition = true;
    }

    bool valid;
    int newAnchorPosition = inputMethodHost()->anchorPosition(valid);
    if (!valid) {
        newAnchorPosition = -1;
    }
    if (newAnchorPosition != d->m_anchorPosition) {
        d->m_anchorPosition = newAnchorPosition;
        emitAnchorPosition = true;
    }

    bool newHasSelection = inputMethodHost()->hasSelection(valid);
    if (!valid) {
        newHasSelection = false;
    }
    if (newHasSelection != d->m_hasSelection) {
        d->m_hasSelection = newHasSelection;
        emitSelection = true;
    }

    int newContentType = inputMethodHost()->contentType(valid);
    if (!valid) {
        newContentType = MaliitQuick::FreeTextContentType;
    }

    if (newContentType != d->m_contentType) {
        d->m_contentType = newContentType;
        emitContentType = true;
    }

    bool newAutoCapitalizationEnabled = inputMethodHost()->autoCapitalizationEnabled(valid);
    if (!valid) {
        newAutoCapitalizationEnabled = true;
    }
    if (newAutoCapitalizationEnabled != d->m_autoCapitalizationEnabled) {
        d->m_autoCapitalizationEnabled = newAutoCapitalizationEnabled;
        emitAutoCapitalization = true;
    }

    bool newPredictionEnabled = inputMethodHost()->predictionEnabled(valid);
    if (!valid) {
        newPredictionEnabled = true;
    }
    if (newPredictionEnabled != d->m_predictionEnabled) {
        d->m_predictionEnabled = newPredictionEnabled;
        emitPredictionEnabled = true;
    }

    bool newHiddenText = inputMethodHost()->hiddenText(valid);
    if (!valid) {
        newHiddenText = false;
    }
    if (newHiddenText != d->m_hiddenText) {
        d->m_hiddenText = newHiddenText;
        emitHiddenText = true;
    }

    if (emitSurroundingText) {
        Q_EMIT surroundingTextChanged();
    }
    if (emitSurroundingTextValid) {
        Q_EMIT surroundingTextValidChanged();
    }
    if (emitCursorPosition) {
        Q_EMIT cursorPositionChanged();
    }
    if (emitAnchorPosition) {
        Q_EMIT anchorPositionChanged();
    }
    if (emitSelection) {
        Q_EMIT hasSelectionChanged();
    }
    if (emitContentType) {
        Q_EMIT contentTypeChanged();
    }
    if (emitAutoCapitalization) {
        Q_EMIT autoCapitalizationChanged();
    }
    if (emitPredictionEnabled) {
        Q_EMIT predictionEnabledChanged();
    }
    if (emitHiddenText) {
        Q_EMIT hiddenTextChanged();
    }

    Q_EMIT editorStateUpdate();
}

void MInputMethodQuick::reset()
{
    Q_EMIT inputMethodReset();
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
        setActive(false);
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
        setActive(false);
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
            setActive(false);
        } else {
            setActive(true);
        }
    }
}


void MInputMethodQuick::propagateScreenSize()
{
    const QSize screenSize(QGuiApplication::primaryScreen()->availableSize());

    Q_EMIT screenWidthChanged(screenSize.width());
    Q_EMIT screenHeightChanged(screenSize.height());
}

int MInputMethodQuick::screenHeight() const
{
    return QGuiApplication::primaryScreen()->availableSize().height();
}

int MInputMethodQuick::screenWidth() const
{
    return QGuiApplication::primaryScreen()->availableSize().width();
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

        Q_EMIT inputMethodAreaChanged(d->inputMethodArea);
        d->syncInputMask();
    }
}

void MInputMethodQuick::setScreenRegion(const QRect &region)
{
    inputMethodHost()->setScreenRegion(region);
}

void MInputMethodQuick::sendPreedit(const QString &text,
                                    const QVariant &preeditFormats,
                                    int replacementStart, int replacementLength, int cursorPos)
{
    QList<Maliit::PreeditTextFormat> formatList;

    if (!preeditFormats.isValid()) {
        // Fallback
        formatList.append(Maliit::PreeditTextFormat(0, text.length(), Maliit::PreeditDefault));

    } else if (preeditFormats.type() == QVariant::Int) {
        // format with one type
        Maliit::PreeditTextFormat format(0, text.length(),
                                         static_cast<Maliit::PreeditFace>(preeditFormats.toInt()));
        formatList.append(format);

    } else if (preeditFormats.type() == QVariant::List) {
        // formatting as list of three ints: type, start, length
        QVariantList list = preeditFormats.toList();

        for (int i = 0; i < list.size(); ++i) {
            QVariantList formatTuple = list.at(i).toList();
            Maliit::PreeditFace face = Maliit::PreeditDefault;
            int start = 0;
            int length = 0;

            if (formatTuple.length() < 3) {
                qWarning() << "MInputMethodQuick.sendPreedit() got formatting tuple with less than three parameters";
                continue;
            }

            face = static_cast<Maliit::PreeditFace>(formatTuple.at(0).toInt());
            start = qBound(0, formatTuple.at(1).toInt(), text.length());
            length = qBound(0, formatTuple.at(2).toInt(), text.length() - start);

            formatList.append(Maliit::PreeditTextFormat(start, length, face));
        }
    }

    inputMethodHost()->sendPreeditString(text, formatList, replacementStart, replacementLength, cursorPos);
}

void MInputMethodQuick::sendKey(int key, int modifiers, const QString &text, int type)
{
    if (type == MaliitQuick::KeyPress || type == MaliitQuick::KeyClick) {
        QKeyEvent event(QEvent::KeyPress, key, (~(Qt::KeyboardModifiers(Qt::NoModifier))) & modifiers, text);
        inputMethodHost()->sendKeyEvent(event);
    }

    if (type == MaliitQuick::KeyRelease || type == MaliitQuick::KeyClick) {
        QKeyEvent event(QEvent::KeyRelease, key, (~(Qt::KeyboardModifiers(Qt::NoModifier))) & modifiers, text);
        inputMethodHost()->sendKeyEvent(event);
    }
}

void MInputMethodQuick::sendCommit(const QString &text, int replaceStart, int replaceLength, int cursorPos)
{
    if (text == "\b") {
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
        inputMethodHost()->sendKeyEvent(event);
    } else
    if ((text == "\r\n") || (text == "\n") || (text == "\r")) {
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
        inputMethodHost()->sendKeyEvent(event);
    } else {
        inputMethodHost()->sendCommitString(text, replaceStart, replaceLength, cursorPos);
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
    sendKey(Qt::Key_Return, 0, "\r", MaliitQuick::KeyClick);
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

bool MInputMethodQuick::surroundingTextValid()
{
    Q_D(MInputMethodQuick);
    return d->m_surroundingTextValid;
}

QString MInputMethodQuick::surroundingText()
{
    // Note: fetching value instead of using member variable for allowing connection side to
    // modify text when sending commit.
    QString text;
    int position;
    inputMethodHost()->surroundingText(text, position);
    return text;
}

int MInputMethodQuick::cursorPosition()
{
    // see ::surroundingText()
    QString text;
    int position;
    inputMethodHost()->surroundingText(text, position);
    return position;
}

int MInputMethodQuick::anchorPosition()
{
    Q_D(MInputMethodQuick);
    return d->m_anchorPosition;
}

bool MInputMethodQuick::hasSelection()
{
    Q_D(MInputMethodQuick);
    return d->m_hasSelection;
}

int MInputMethodQuick::contentType()
{
    Q_D(MInputMethodQuick);
    return d->m_contentType;
}

bool MInputMethodQuick::predictionEnabled()
{
    Q_D(MInputMethodQuick);
    return d->m_predictionEnabled;
}

bool MInputMethodQuick::autoCapitalizationEnabled()
{
    Q_D(MInputMethodQuick);
    return d->m_autoCapitalizationEnabled;
}

bool MInputMethodQuick::hiddenText()
{
    Q_D(MInputMethodQuick);
    return d->m_hiddenText;
}
