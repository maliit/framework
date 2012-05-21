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

#ifndef MIMSERVERLOGIC_H
#define MIMSERVERLOGIC_H

#include <QObject>
#include <QRegion>
#include <QSharedPointer>

#include <tr1/functional>

//! Walks over widget hierarchy, if used with
//! visitWidgetHierarchy. Return true if children of
//! current widget shall be visited, too.
typedef std::tr1::function<bool (QWidget *)> WidgetVisitor;

void visitWidgetHierarchy(WidgetVisitor visitor, QWidget *widget);

namespace Maliit {
namespace Server {
    class AbstractSurfaceGroupFactory;
}
}

class MImAbstractServerLogic : public QObject
{
    Q_OBJECT

public:
    explicit MImAbstractServerLogic(QObject *parent = 0);
    virtual ~MImAbstractServerLogic() = 0;

    //! Return factory for creating surface groups
    virtual QSharedPointer<Maliit::Server::AbstractSurfaceGroupFactory> surfaceGroupFactory() const = 0;

public Q_SLOTS:
    virtual void inputPassthrough(const QRegion &region) = 0;
    virtual void appOrientationAboutToChange(int toAngle) = 0;
    virtual void appOrientationChangeFinished(int toAngle) = 0;
    virtual void applicationFocusChanged(WId remoteWinId) = 0;
    virtual void pluginLoaded() = 0;

Q_SIGNALS:
    void applicationWindowGone();
};

#endif // MIMSERVERLOGIC_H
