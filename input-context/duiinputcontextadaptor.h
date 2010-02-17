/* * This file is part of dui-im-framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
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

#ifndef DUIINPUTCONTEXTADAPTOR_H
#define DUIINPUTCONTEXTADAPTOR_H

#include <QString>
#include <QDBusAbstractAdaptor>
#include <QList>
#include <QMap>
#include <QRect>

class DuiInputContext;


class DuiInputContextAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.maemo.duiinputcontext1")
    // FIXME: introspection

public:
    DuiInputContextAdaptor(DuiInputContext *parent);
    virtual ~DuiInputContextAdaptor();

public slots:
    virtual Q_NOREPLY void activationLostEvent();
    virtual Q_NOREPLY void imInitiatedHide();
    virtual Q_NOREPLY void commitString(const QString &string);
    virtual Q_NOREPLY void updatePreedit(const QString &string, int preeditFace);
    virtual Q_NOREPLY void keyEvent(int type, int key, int modifiers, const QString &text,
                                    bool autoRepeat, int count);
    virtual Q_NOREPLY void updateInputMethodArea(const QList<QVariant> &data);
    virtual Q_NOREPLY void setGlobalCorrectionEnabled(bool);
    virtual Q_NOREPLY void copy();
    virtual Q_NOREPLY void paste();
    virtual Q_NOREPLY void setComposingTextInput(bool);
    virtual Q_NOREPLY void addRedirectedKey(int keyCode, bool eatInBetweenKeys, bool eatItself);
    virtual Q_NOREPLY void removeRedirectedKey(int keyCode);
    virtual Q_NOREPLY void setNextKeyRedirected(bool);

    // valid is out parameter for value validity
    virtual int contentType(bool &valid);
    virtual bool correctionEnabled(bool &valid);
    virtual bool predictionEnabled(bool &valid);
    virtual bool autoCapitalizationEnabled(bool &valid);
    virtual bool surroundingText(QString &text, int &cursorPosition);
    virtual bool hasSelection(bool &valid);
    virtual int inputMethodMode(bool &valid);
    virtual QRect preeditRectangle(bool &valid);

private:
    DuiInputContext *owner;
};


#endif
