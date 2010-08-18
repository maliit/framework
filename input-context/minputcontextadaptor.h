/* * This file is part of meego-im-framework *
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

#ifndef MINPUTCONTEXTADAPTOR_H
#define MINPUTCONTEXTADAPTOR_H

#include <QString>
#include <QDBusAbstractAdaptor>
#include <QList>
#include <QMap>
#include <QRect>

class MInputContext;


class MInputContextAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.meego.inputmethod.inputcontext1")

    // FIXME: introspection

public:
    explicit MInputContextAdaptor(MInputContext *parent);
    virtual ~MInputContextAdaptor();

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
    virtual Q_NOREPLY void setRedirectKeys(bool enabled);
    virtual Q_NOREPLY void setDetectableAutoRepeat(bool enabled);

    // valid is out parameter for value validity
    virtual QRect preeditRectangle(bool &valid);

private:
    MInputContext *owner;
};


#endif
