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

#include "mindicatorserviceclient.h"

#include <QDBusInterface>
#include <QDBusServiceWatcher>
#include <QDebug>

namespace {
    const char * const DBusIndicatorServiceName = "com.meego.core.MInputMethodStatusIndicator";
    const char * const DBusIndicatorPath = "/inputmethodstatusindicator";
    const char * const DBusIndicatorInterface = "com.meego.core.MInputMethodStatusIndicator";
};


MIndicatorServiceClient::MIndicatorServiceClient(QObject *parent)
    : QObject(parent),
      indicatorIface(0),
      indicatorServiceWatcher(new QDBusServiceWatcher(DBusIndicatorServiceName,
                                                      QDBusConnection::sessionBus(),
                                                      QDBusServiceWatcher::WatchForOwnerChange,
                                                      this))
{
    connectToIndicatorDBus();
    connect(indicatorServiceWatcher, SIGNAL(serviceOwnerChanged(QString, QString, QString)),
            this, SLOT(handleIndicatorServiceChanged(QString, QString, QString)));

    indicatorMap.insert(MInputMethod::NoIndicator, "");
    indicatorMap.insert(MInputMethod::LatinLowerIndicator, "icon-s-status-latin-lowercase");
    indicatorMap.insert(MInputMethod::LatinUpperIndicator, "icon-s-status-latin-uppercase");
    indicatorMap.insert(MInputMethod::LatinLockedIndicator, "icon-s-status-latin-caps");
    indicatorMap.insert(MInputMethod::CyrillicLowerIndicator, "icon-s-status-cyrillic-lowercase");
    indicatorMap.insert(MInputMethod::CyrillicUpperIndicator, "icon-s-status-cyrillic-uppercase");
    indicatorMap.insert(MInputMethod::CyrillicLockedIndicator, "icon-s-status-cyrillic-caps");
    indicatorMap.insert(MInputMethod::ArabicIndicator, "icon-s-status-arabic");
    indicatorMap.insert(MInputMethod::PinyinIndicator, "icon-s-status-pinyin");
    indicatorMap.insert(MInputMethod::ZhuyinIndicator, "icon-s-status-zhuyin");
    indicatorMap.insert(MInputMethod::CangjieIndicator, "icon-s-status-cangjie");
    indicatorMap.insert(MInputMethod::NumAndSymLatchedIndicator, "icon-s-status-number");
    indicatorMap.insert(MInputMethod::NumAndSymLockedIndicator, "icon-s-status-number-locked");
    indicatorMap.insert(MInputMethod::DeadKeyAcuteIndicator, "icon-s-status-acute");
    indicatorMap.insert(MInputMethod::DeadKeyCaronIndicator, "icon-s-status-caron");
    indicatorMap.insert(MInputMethod::DeadKeyCircumflexIndicator, "icon-s-status-circumflex");
    indicatorMap.insert(MInputMethod::DeadKeyDiaeresisIndicator, "icon-s-status-diaeresis");
    indicatorMap.insert(MInputMethod::DeadKeyGraveIndicator, "icon-s-status-grave");
    indicatorMap.insert(MInputMethod::DeadKeyTildeIndicator, "icon-s-status-tilde");
}

MIndicatorServiceClient::~MIndicatorServiceClient()
{
    delete indicatorIface;
}

void MIndicatorServiceClient::setInputModeIndicator(MInputMethod::InputModeIndicator mode)
{
    if (indicatorIface) {
        indicatorIface->call(QDBus::NoBlock, "setIconID", indicatorIconID(mode));
    }
}

void MIndicatorServiceClient::connectToIndicatorDBus()
{
    qDebug() << __PRETTY_FUNCTION__;
    QDBusConnection connection = QDBusConnection::sessionBus();

    if (!connection.isConnected()) {
        qWarning() << "Cannot connect to the DBus session bus";
        return;
    }

    indicatorIface = new QDBusInterface(DBusIndicatorServiceName, DBusIndicatorPath,
                                        DBusIndicatorInterface, connection);

    if (!indicatorIface->isValid()) {
        qWarning() << "MInputContextDBusConnection was unable to connect to indicator server: "
                   << connection.lastError().message();
        delete indicatorIface;
        indicatorIface = 0;
    }
}

void MIndicatorServiceClient::handleIndicatorServiceChanged(const QString &serviceName,
                                                            const QString &oldOwner,
                                                            const QString &newOwner)
{
    Q_UNUSED(oldOwner);

    qDebug() << __PRETTY_FUNCTION__;

    if (serviceName != DBusIndicatorServiceName) {
        return;
    }

    if (indicatorIface && !indicatorIface->isValid()) {
        delete indicatorIface;
        indicatorIface = 0;
    }

    if (!newOwner.isEmpty()) {
        connectToIndicatorDBus();
    }
}

QString MIndicatorServiceClient::indicatorIconID(MInputMethod::InputModeIndicator mode)
{
    return indicatorMap.value(mode);
}

