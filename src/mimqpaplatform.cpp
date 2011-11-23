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

#include "mimqpaplatform.h"

#include "mimpluginsproxywidget.h"

#include <QDebug>
#include <QWidget>

MImQPAPlatform::MImQPAPlatform() :
    QObject(0),
    mProxyWidget(new MImPluginsProxyWidget)
{
}

void MImQPAPlatform::inputPassthrough(const QRegion &region)
{
    qDebug() << __PRETTY_FUNCTION__;

    if (region.isEmpty())
        mProxyWidget->hide();
    else
        mProxyWidget->show();
}

QWidget *MImQPAPlatform::pluginsProxyWidget()
{
    return mProxyWidget.get();
}

