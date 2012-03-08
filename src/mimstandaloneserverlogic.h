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

#include "mimabstractserverlogic.h"

class QWidget;

// Non-X11 specific standalone server
//
// For QPA the toplevel window is a MImPluginsProxyWidget containing all plugin
// widgets. The MImStandaloneServerLogic shows/hides that toplevel window when
// required.
class MImStandaloneServerLogic : public MImAbstractServerLogic
{
    Q_OBJECT
public:
    explicit MImStandaloneServerLogic();
    ~MImStandaloneServerLogic();

    //! reimpl
    QWidget *pluginsProxyWidget() const;

public Q_SLOTS:
    //! reimpl
    virtual void inputPassthrough(const QRegion &region = QRegion());
    virtual void appOrientationAboutToChange(int toAngle);
    virtual void appOrientationChangeFinished(int toAngle);
    virtual void applicationFocusChanged(WId remoteWinId);
    virtual void pluginLoaded();
    //! reimpl_end

private:
    std::auto_ptr<QWidget> mProxyWidget;
};

#endif // MIMQPAPLATFORM_H
