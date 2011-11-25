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

#include "mimmeegoindicator.h"

#include <QDBusInterface>
#include <QDBusServiceWatcher>
#include <QDebug>

namespace {
    const char * const DBusIndicatorServiceName = "com.meego.core.MInputMethodStatusIndicator";
    const char * const DBusIndicatorPath = "/inputmethodstatusindicator";
    const char * const DBusIndicatorInterface = "com.meego.core.MInputMethodStatusIndicator";
};


MImMeegoIndicator::MImMeegoIndicator(QObject *parent)
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

    indicatorMap.insert(Maliit::NoIndicator, "");
    indicatorMap.insert(Maliit::LatinLowerIndicator, "icon-s-status-latin-lowercase");
    indicatorMap.insert(Maliit::LatinUpperIndicator, "icon-s-status-latin-uppercase");
    indicatorMap.insert(Maliit::LatinLockedIndicator, "icon-s-status-latin-caps");
    indicatorMap.insert(Maliit::CyrillicLowerIndicator, "icon-s-status-cyrillic-lowercase");
    indicatorMap.insert(Maliit::CyrillicUpperIndicator, "icon-s-status-cyrillic-uppercase");
    indicatorMap.insert(Maliit::CyrillicLockedIndicator, "icon-s-status-cyrillic-caps");
    indicatorMap.insert(Maliit::ArabicIndicator, "icon-s-status-arabic");
    indicatorMap.insert(Maliit::PinyinIndicator, "icon-s-status-pinyin");
    indicatorMap.insert(Maliit::ZhuyinIndicator, "icon-s-status-zhuyin");
    indicatorMap.insert(Maliit::CangjieIndicator, "icon-s-status-cangjie");
    indicatorMap.insert(Maliit::NumAndSymLatchedIndicator, "icon-s-status-number");
    indicatorMap.insert(Maliit::NumAndSymLockedIndicator, "icon-s-status-number-locked");
    indicatorMap.insert(Maliit::DeadKeyAcuteIndicator, "icon-s-status-acute");
    indicatorMap.insert(Maliit::DeadKeyCaronIndicator, "icon-s-status-caron");
    indicatorMap.insert(Maliit::DeadKeyCircumflexIndicator, "icon-s-status-circumflex");
    indicatorMap.insert(Maliit::DeadKeyDiaeresisIndicator, "icon-s-status-diaeresis");
    indicatorMap.insert(Maliit::DeadKeyGraveIndicator, "icon-s-status-grave");
    indicatorMap.insert(Maliit::DeadKeyTildeIndicator, "icon-s-status-tilde");
}

MImMeegoIndicator::~MImMeegoIndicator()
{
    delete indicatorIface;
}

void MImMeegoIndicator::setInputModeIndicator(Maliit::InputModeIndicator mode)
{
    if (indicatorIface) {
        indicatorIface->call(QDBus::NoBlock, "setIconID", indicatorIconID(mode));
    }
}

void MImMeegoIndicator::connectToIndicatorDBus()
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

void MImMeegoIndicator::handleIndicatorServiceChanged(const QString &serviceName,
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

QString MImMeegoIndicator::indicatorIconID(Maliit::InputModeIndicator mode)
{
    return indicatorMap.value(mode);
}

