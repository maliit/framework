/* 
 * Copyright (c) 2017 Jan Arne Petersen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 *
 */

#ifndef MALIIT_FRAMEWORK_STANDALONEINPUTMETHODHOST_H
#define MALIIT_FRAMEWORK_STANDALONEINPUTMETHODHOST_H

#include <maliit/plugins/abstractinputmethodhost.h>

class MInputContextConnection;

namespace Maliit {

class WindowGroup;

class StandaloneInputMethodHost : public MAbstractInputMethodHost
{
    Q_OBJECT
public:
    StandaloneInputMethodHost(MInputContextConnection *connection,
                              WindowGroup *windowGroup);

    int contentType(bool &valid) override;
    bool correctionEnabled(bool &valid) override;
    bool predictionEnabled(bool &valid) override;
    bool autoCapitalizationEnabled(bool &valid) override;
    bool surroundingText(QString &text, int &cursorPosition) override;
    bool hasSelection(bool &valid) override;
    int inputMethodMode(bool &valid) override;
    QRect preeditRectangle(bool &valid) override;
    QRect cursorRectangle(bool &valid) override;
    int anchorPosition(bool &valid) override;
    QString selection(bool &valid) override;
    void registerWindow(QWindow *window, Maliit::Position position) override;
    void sendPreeditString(const QString &string,
                           const QList<Maliit::PreeditTextFormat> &preeditFormats,
                           int replacementStart,
                           int replacementLength,
                           int cursorPos) override;
    void sendCommitString(const QString &string, int replaceStart, int replaceLength, int cursorPos) override;
    void sendKeyEvent(const QKeyEvent &keyEvent, Maliit::EventRequestType requestType) override;
    void notifyImInitiatedHiding() override;
    void invokeAction(const QString &action, const QKeySequence &sequence) override;
    void setRedirectKeys(bool enabled) override;
    void setDetectableAutoRepeat(bool enabled) override;
    void setGlobalCorrectionEnabled(bool enabled) override;
    void switchPlugin(Maliit::SwitchDirection direction) override;
    void switchPlugin(const QString &pluginName) override;
    void setScreenRegion(const QRegion &region, QWindow *window) override;
    void setInputMethodArea(const QRegion &region, QWindow *window) override;
    void setSelection(int start, int length) override;
    void setOrientationAngleLocked(bool lock) override;
    QList<MImPluginDescription> pluginDescriptions(Maliit::HandlerState state) const override;
    Maliit::Plugins::AbstractPluginSetting *registerPluginSetting(const QString &key,
                                                                  const QString &description,
                                                                  Maliit::SettingEntryType type,
                                                                  const QVariantMap &attributes) override;
    bool hiddenText(bool &valid) override;
    void setLanguage(const QString &language) override;
    QVariant inputMethodQuery(Qt::InputMethodQuery query, const QVariant &argument) const override;
private:
    MInputContextConnection *mConnection;
    WindowGroup *mWindowGroup;
};

}

#endif //MALIIT_FRAMEWORK_STANDALONEINPUTMETHODHOST_H
