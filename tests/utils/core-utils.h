/* * This file is part of maliit-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CORE_UTILS_H__
#define CORE_UTILS_H__

#include <QString>
#include <QObject>

#include "abstractsurfacegroup.h"
#include "abstractsurfacegroupfactory.h"

namespace MaliitTestUtils {

    bool isTestingInSandbox();
    QString getTestPluginPath();
    QString getTestDataPath();
    void waitForSignal(const QObject* object, const char* signal, int timeout);
    void waitAndProcessEvents(int waitTime);

    class TestSurfaceGroup : public Maliit::Server::AbstractSurfaceGroup {
    public:
        TestSurfaceGroup() {}

        Maliit::Plugins::AbstractSurfaceFactory *factory() { return 0; }

        void activate() {}
        void deactivate() {}

        void setRotation(Maliit::OrientationAngle) {}
    };

    class TestSurfaceGroupFactory : public Maliit::Server::AbstractSurfaceGroupFactory {
    public:
        TestSurfaceGroupFactory() {}

        QSharedPointer<Maliit::Server::AbstractSurfaceGroup> createSurfaceGroup()
        {
            return QSharedPointer<Maliit::Server::AbstractSurfaceGroup>(new TestSurfaceGroup);
        }
    };
}

#endif // CORE_UTILS_H__
