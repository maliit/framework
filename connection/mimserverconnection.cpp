/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "mimserverconnection.h"

/* Dummy class that does nothing. */
MImServerConnection::MImServerConnection(QObject *parent)
    : QObject(parent)
    , d(0)
{
    Q_UNUSED(parent);
}

void MImServerConnection::activateContext()
{}

void MImServerConnection::showInputMethod()
{}

void MImServerConnection::hideInputMethod()
{}

void MImServerConnection::mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect)
{
    Q_UNUSED(pos);
    Q_UNUSED(preeditRect);
}

void MImServerConnection::setPreedit(const QString &text, int cursorPos)
{
    Q_UNUSED(text);
    Q_UNUSED(cursorPos);
}

void MImServerConnection::updateWidgetInformation(const QMap<QString, QVariant> &stateInformation, bool focusChanged)
{
    Q_UNUSED(stateInformation);
    Q_UNUSED(focusChanged);
}

void MImServerConnection::reset(bool requireSynchronization)
{
    Q_UNUSED(requireSynchronization);
}

bool MImServerConnection::pendingResets()
{
    return false;
}

void MImServerConnection::appOrientationAboutToChange(int angle)
{
    Q_UNUSED(angle);
}

void MImServerConnection::appOrientationChanged(int angle)
{
    Q_UNUSED(angle);
}

void MImServerConnection::setCopyPasteState(bool copyAvailable, bool pasteAvailable)
{
    Q_UNUSED(copyAvailable);
    Q_UNUSED(pasteAvailable);
}

void MImServerConnection::processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                          Qt::KeyboardModifiers modifiers,
                                          const QString &text, bool autoRepeat, int count,
                                          quint32 nativeScanCode, quint32 nativeModifiers,
                                          unsigned long time)
{
    Q_UNUSED(keyType);
    Q_UNUSED(keyCode);
    Q_UNUSED(modifiers);
    Q_UNUSED(text);
    Q_UNUSED(autoRepeat);
    Q_UNUSED(count);
    Q_UNUSED(nativeScanCode);
    Q_UNUSED(nativeModifiers);
    Q_UNUSED(time);
}

void MImServerConnection::registerAttributeExtension(int id, const QString &fileName)
{
    Q_UNUSED(id);
    Q_UNUSED(fileName);
}

void MImServerConnection::unregisterAttributeExtension(int id)
{
    Q_UNUSED(id);
}

void MImServerConnection::setExtendedAttribute(int id, const QString &target, const QString &targetItem,
                                               const QString &attribute, const QVariant &value)
{
    Q_UNUSED(id);
    Q_UNUSED(target);
    Q_UNUSED(targetItem);
    Q_UNUSED(attribute);
    Q_UNUSED(value);
}
