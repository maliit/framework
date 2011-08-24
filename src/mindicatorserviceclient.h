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

#ifndef MINDICATORSERVICECLIENT_H
#define MINDICATORSERVICECLIENT_H


#include <QObject>
#include <QMap>

#include "minputmethodnamespace.h"

class QString;
class QDBusInterface;
class QDBusServiceWatcher;

//! \internal
/*!
 * \brief Provides interface for setting the system keyboard indicator state.
 */
class MIndicatorServiceClient: public QObject
{
    Q_OBJECT

public:
    explicit MIndicatorServiceClient(QObject *parent = 0);
    ~MIndicatorServiceClient();

    void setInputModeIndicator(MInputMethod::InputModeIndicator mode);

private Q_SLOTS:
    void handleIndicatorServiceChanged(const QString &serviceName, const QString &oldOwner,
                                       const QString &newOwner);
private:
    Q_DISABLE_COPY(MIndicatorServiceClient)

    void connectToIndicatorDBus();

    //! map the InputModeIndicator to icon id
    QString indicatorIconID(MInputMethod::InputModeIndicator mode);

    QDBusInterface *indicatorIface; // indicator server interface
    QDBusServiceWatcher *indicatorServiceWatcher;
    QMap<MInputMethod::InputModeIndicator, QString> indicatorMap;
};
//! \internal_end

#endif
