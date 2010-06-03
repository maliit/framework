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

#ifndef MIMSETTINGSAPPLET_H
#define MIMSETTINGSAPPLET_H

#include <QObject>
#include <DcpAppletIf>
class DcpWidget;
class MAction;

class MImSettingsApplet : public QObject, public DcpAppletIf
{
    Q_OBJECT
    Q_INTERFACES(DcpAppletIf)
public:
    //! reimp
    virtual ~MImSettingsApplet();
    virtual void init();
    virtual DcpWidget* constructWidget(int widgetId);
    virtual QString title() const;
    virtual QVector<MAction*> viewMenuItems();
    virtual DcpBrief* constructBrief(int partId);
    //! reimp_end
};

#endif
