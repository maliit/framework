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

#ifndef MIMQPAPLATFORM_H
#define MIMQPAPLATFORM_H

#include <QObject>
#include <QRegion>
#include <QWeakPointer>

#include <memory>

class QWidget;

class MImQPAPlatform : public QObject
{
    Q_OBJECT
public:
    explicit MImQPAPlatform();

    QWidget *pluginsProxyWidget();

public Q_SLOTS:
    //! Set window ID for given region
    void inputPassthrough(const QRegion &region = QRegion());

private:
    std::auto_ptr<QWidget> mProxyWidget;
};

#endif // MIMQPAPLATFORM_H
