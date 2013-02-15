/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
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

/* In Qt5, QSKIP takes one parameter; a description of the test being skipped.
 * In Qt4, QSKIP takes two; a description and a mode indicating whether to skip
 * just this one test or all of them.
 *
 * This is obviously an API incompatibility. To enable maliit to build with
 * both Qt4 and Qt5, we define a compatibility macro that calls QSKIP with the
 * appropriate parameters. This approaach was taken by KDE in kdelibs (see
 * commit 59c089ab7bb187b841b386da2153a2c1699d57c4)
 */

#pragma message("Port to Qt5 version of QSKIP_PORTING")
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#define SkipSingle 0
#define SkipAll 0
#define QSKIP_PORTING(message, argument) QSKIP(message)
#else
#define QSKIP_PORTING(message, argument) QSKIP(message, argument)
#endif

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
