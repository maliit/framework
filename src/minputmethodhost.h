/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MINPUTMETHODHOST_H
#define MINPUTMETHODHOST_H

#include <maliit/plugins/abstractinputmethodhost.h>

class MInputContextConnection;
class MIMPluginManager;
class MAbstractInputMethod;

namespace Maliit
{

class WindowGroup;

} // namespace Maliit

class QRegion;

using Maliit::Plugins::AbstractPluginSetting;

/*! \internal
 * \ingroup maliitserver
 * \brief Interface implementation for connecting input method instances to the environment.
 */
class MInputMethodHost: public MAbstractInputMethodHost
{
    Q_OBJECT

public:
    MInputMethodHost(const QSharedPointer<MInputContextConnection>& inputContextConnection,
                     MIMPluginManager *pluginManager,
                     const QSharedPointer<Maliit::WindowGroup> &window_group, const QString &plugin,
                     const QString &description);
    virtual ~MInputMethodHost();

    //! if enabled, the plugin associated with this host are allowed to communicate
    void setEnabled(bool enabled);

    //! associate input method with this host instance.
    //! Multiple calls is (currently) undefined behavior.
    void setInputMethod(MAbstractInputMethod *inputMethod);

    // \reimp
    virtual int contentType(bool &valid);
    virtual bool correctionEnabled(bool &valid);
    virtual bool predictionEnabled(bool &valid);
    virtual bool autoCapitalizationEnabled(bool &valid);
    virtual bool surroundingText(QString &text, int &cursorPosition);
    virtual bool hasSelection(bool &valid);
    virtual int inputMethodMode(bool &valid);
    virtual QRect preeditRectangle(bool &valid);
    virtual QRect cursorRectangle(bool &valid);
    virtual int anchorPosition(bool &valid);
    virtual bool hiddenText(bool &valid);
    virtual QString selection(bool &valid);
    virtual void registerWindow (QWindow *window,
                                 Maliit::Position position);
    virtual void sendPreeditString(const QString &string,
                                   const QList<Maliit::PreeditTextFormat> &preeditFormats,
                                   int replacementStart = 0, int replacementLength = 0,
                                   int cursorPos = -1);
    virtual void sendCommitString(const QString &string, int replaceStart = 0,
                                  int replaceLength = 0, int cursorPos = -1);
    virtual void sendKeyEvent(const QKeyEvent &keyEvent,
                              Maliit::EventRequestType requestType
                               = Maliit::EventRequestBoth);
    virtual void notifyImInitiatedHiding();
    virtual void invokeAction(const QString &action,
                            const QKeySequence &sequence);
    virtual void setRedirectKeys(bool enabled);
    virtual void setDetectableAutoRepeat(bool enabled);
    virtual void setGlobalCorrectionEnabled(bool enabled);

    virtual void switchPlugin(Maliit::SwitchDirection direction);
    virtual void switchPlugin(const QString &pluginName);
    virtual void setScreenRegion(const QRegion &region);
    virtual void setInputMethodArea(const QRegion &region);
    virtual void setSelection(int start, int length);
    virtual QList<MImPluginDescription> pluginDescriptions(Maliit::HandlerState state) const;
    virtual int preeditClickPos(bool &valid) const;
    virtual QList<MImSubViewDescription> surroundingSubViewDescriptions(Maliit::HandlerState state) const;
    virtual void setLanguage(const QString &language);

    //! Only empty implementation provided.
    virtual void setOrientationAngleLocked(bool lock);
    virtual AbstractPluginSetting *registerPluginSetting(const QString &key,
                                                         const QString &description,
                                                         Maliit::SettingEntryType type,
                                                         const QVariantMap &attributes);
    // \reimp_end

private:
    Q_DISABLE_COPY(MInputMethodHost)

    QSharedPointer<MInputContextConnection> connection;
    MIMPluginManager *pluginManager;
    MAbstractInputMethod *inputMethod;
    bool enabled;
    QString pluginId;
    QString pluginDescription;
    QSharedPointer<Maliit::WindowGroup> mWindowGroup;
};

//! \internal_end

#endif
