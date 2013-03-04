/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Canonical Ltd
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef MINPUTCONTEXTWESTONIMPROTOCOLCONNECTION_H
#define MINPUTCONTEXTWESTONIMPROTOCOLCONNECTION_H

#include <maliit/namespace.h>
#include "minputcontextconnection.h"

#include <QtCore>

class MInputContextWestonIMProtocolConnectionPrivate;

/*! \internal
 * \ingroup maliitserver
 * \brief Input method communication implementation between the Weston
 * and the input method server.
 */
class MInputContextWestonIMProtocolConnection : public MInputContextConnection
{
    Q_OBJECT
    Q_DISABLE_COPY(MInputContextWestonIMProtocolConnection)
    Q_DECLARE_PRIVATE(MInputContextWestonIMProtocolConnection)

public:
    explicit MInputContextWestonIMProtocolConnection();
    virtual ~MInputContextWestonIMProtocolConnection();

//    virtual bool focusState(bool &valid);
//    virtual int contentType(bool &valid);
//    virtual bool correctionEnabled(bool &valid);
//    virtual bool predictionEnabled(bool &valid);
//    virtual bool autoCapitalizationEnabled(bool &valid);
//    virtual bool surroundingText(QString &text,
//                                 int &cursor_position);
//    virtual bool hasSelection(bool &valid);
//    virtual int inputMethodMode(bool &valid);
//    virtual QRect preeditRectangle(bool &valid);
//    virtual QRect cursorRectangle(bool &valid);
//    virtual bool hiddenText(bool &valid);
    virtual void sendPreeditString(const QString &string,
                                   const QList<Maliit::PreeditTextFormat> &preedit_formats,
                                   int replacement_start = 0,
                                   int replacement_length = 0,
                                   int cursor_pos = -1);
    virtual void sendCommitString(const QString &string,
                                  int replace_start = 0,
                                  int replace_length = 0,
                                  int cursor_pos = -1);
    virtual void sendKeyEvent(const QKeyEvent &key_event,
                              Maliit::EventRequestType request_type);
//    virtual void notifyImInitiatedHiding();
//    virtual void invokeAction(const QString &action,
//                              const QKeySequence &sequence);
//    virtual void setRedirectKeys(bool enabled);
//    virtual void setDetectableAutoRepeat(bool enabled);
//    virtual void setGlobalCorrectionEnabled(bool enabled);
//    virtual void setSelection(int start,
//                              int length);
//    virtual int anchorPosition(bool &valid);
//    virtual int preeditClickPos(bool &valid) const;
    virtual QString selection(bool &valid);
//    virtual void setLanguage(const QString &language);
//    virtual void sendActivationLostEvent();
//
//public Q_SLOTS:
//    virtual void updateInputMethodArea(const QRegion &region);
//    virtual void notifyExtendedAttributeChanged(int id,
//                                                const QString &target,
//                                                const QString &target_item,
//                                                const QString &attribute,
//                                                const QVariant &value);
//    virtual void notifyExtendedAttributeChanged(const QList<int> &client_ids,
//                                                int id,
//                                                const QString &target,
//                                                const QString &target_item,
//                                                const QString &attribute,
//                                                const QVariant &value);
//    virtual void pluginSettingsLoaded(int client_id,
//                                      const QList<MImPluginSettingsInfo> &info);

private:
    const QScopedPointer<MInputContextWestonIMProtocolConnectionPrivate> d_ptr;
};
//! \internal_end

#endif
