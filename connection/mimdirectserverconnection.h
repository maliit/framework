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

#ifndef MIMDIRECTSERVERCONNECTION_H
#define MIMDIRECTSERVERCONNECTION_H

#include "mimserverconnection.h"

class MInputContextConnection;
class MImInputContextDirectConnection;

class MImDirectServerConnection : public MImServerConnection
{
    Q_OBJECT

public:
    //! \brief Constructor
    explicit MImDirectServerConnection(QObject *parent = 0);

    void connectTo(MImInputContextDirectConnection *icConnection);

    //! reimpl
    virtual void activateContext();

    virtual void showInputMethod();

    virtual void hideInputMethod();

    virtual void mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect);

    virtual void setPreedit(const QString &text, int cursorPos);

    virtual void updateWidgetInformation(const QMap<QString, QVariant> &stateInformation,
                                         bool focusChanged);

    virtual void reset(bool requireSynchronization);
    virtual bool pendingResets();

    virtual void appOrientationAboutToChange(int angle);

    virtual void appOrientationChanged(int angle);

    virtual void setCopyPasteState(bool copyAvailable, bool pasteAvailable);

    virtual void processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                 Qt::KeyboardModifiers modifiers,
                                 const QString &text, bool autoRepeat, int count,
                                 quint32 nativeScanCode, quint32 nativeModifiers, unsigned long time);

    virtual void registerAttributeExtension(int id, const QString &fileName);

    virtual void unregisterAttributeExtension(int id);

    virtual void setExtendedAttribute(int id, const QString &target, const QString &targetItem,
                                      const QString &attribute, const QVariant &value);
    //! reimpl end

private:
    Q_DISABLE_COPY(MImDirectServerConnection)

    MInputContextConnection *mIcConnection;
    unsigned int mConnectionId;
};

#endif // MIMDIRECTSERVERCONNECTION_H
