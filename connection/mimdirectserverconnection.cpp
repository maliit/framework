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

#include "mimdirectserverconnection.h"

#include "miminputcontextdirectconnection.h"

namespace {
    const int firstConnectionId = 1;
}

MImDirectServerConnection::MImDirectServerConnection(QObject *parent)
    : MImServerConnection(parent)
    , mIcConnection(0)
    , mConnectionId(firstConnectionId)
{
    /* FIXME: Don't hardcode mConnectionId
    Ideally it should be possible to have multiple IC (and thus server connection)
    in one process, talking to one imServer, for testing purposes. */
}

void MImDirectServerConnection::connectTo(MImInputContextDirectConnection *icConnection)
{
    if (!icConnection) {
        qCritical() << __PRETTY_FUNCTION__ << "icConnection cannot be NULL";
        return;
    }

    if (icConnection != mIcConnection) {
        mIcConnection = icConnection;
        icConnection->connectTo(this);
    }
}

void MImDirectServerConnection::activateContext()
{
    mIcConnection->activateContext(mConnectionId);
}

void MImDirectServerConnection::showInputMethod()
{
    mIcConnection->showInputMethod(mConnectionId);
}

void MImDirectServerConnection::hideInputMethod()
{
    mIcConnection->hideInputMethod(mConnectionId);
}

void MImDirectServerConnection::mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect)
{
    mIcConnection->mouseClickedOnPreedit(mConnectionId, pos, preeditRect);
}

void MImDirectServerConnection::setPreedit(const QString &text, int cursorPos)
{
    mIcConnection->setPreedit(mConnectionId, text, cursorPos);
}

void MImDirectServerConnection::updateWidgetInformation(const QMap<QString, QVariant> &stateInformation,
                                                        bool focusChanged)
{
    mIcConnection->updateWidgetInformation(mConnectionId, stateInformation, focusChanged);
}

void MImDirectServerConnection::reset(bool requireSynchronization)
{
    Q_UNUSED(requireSynchronization);
    mIcConnection->reset(mConnectionId);
}

bool MImDirectServerConnection::pendingResets()
{
    return MImServerConnection::pendingResets();
}

void MImDirectServerConnection::appOrientationAboutToChange(int angle)
{
    mIcConnection->receivedAppOrientationAboutToChange(mConnectionId, angle);
}

void MImDirectServerConnection::appOrientationChanged(int angle)
{
    mIcConnection->receivedAppOrientationChanged(mConnectionId, angle);
}

void MImDirectServerConnection::setCopyPasteState(bool copyAvailable, bool pasteAvailable)
{
    mIcConnection->setCopyPasteState(mConnectionId, copyAvailable, pasteAvailable);
}

void MImDirectServerConnection::processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                                Qt::KeyboardModifiers modifiers,
                                                const QString &text, bool autoRepeat, int count,
                                                quint32 nativeScanCode, quint32 nativeModifiers,
                                                unsigned long time)
{
    mIcConnection->processKeyEvent(mConnectionId, keyType, keyCode, modifiers, text,
                                   autoRepeat, count, nativeScanCode, nativeModifiers,
                                   time);
}

void MImDirectServerConnection::registerAttributeExtension(int id, const QString &fileName)
{
    mIcConnection->registerAttributeExtension(mConnectionId, id, fileName);
}

void MImDirectServerConnection::unregisterAttributeExtension(int id)
{
    mIcConnection->unregisterAttributeExtension(mConnectionId, id);
}

void MImDirectServerConnection::setExtendedAttribute(int id, const QString &target, const QString &targetItem,
                                                     const QString &attribute, const QVariant &value)
{
    mIcConnection->setExtendedAttribute(mConnectionId, id, target, targetItem, attribute, value);
}

void MImDirectServerConnection::loadPluginSettings(const QString &descriptionLanguage)
{
    mIcConnection->loadPluginSettings(mConnectionId, descriptionLanguage);
}
