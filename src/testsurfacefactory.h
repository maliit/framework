/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Openismus GmbH
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef MALIIT_SERVER_TESTSURFACEFACTORY_H
#define MALIIT_SERVER_TESTSURFACEFACTORY_H

#include <maliit/plugins/abstractsurface.h>
#include <maliit/plugins/abstractwidgetssurface.h>
#include <QSharedPointer>

namespace Maliit {
namespace Server {

QSharedPointer<Maliit::Plugins::AbstractSurface> createTestSurface(Maliit::Plugins::AbstractSurface::Options options, const QSharedPointer<Maliit::Plugins::AbstractSurface> &parent = QSharedPointer<Maliit::Plugins::AbstractSurface>());

QSharedPointer<Maliit::Plugins::AbstractGraphicsViewSurface> createTestGraphicsViewSurface(const QSharedPointer<Maliit::Plugins::AbstractSurface> &parent = QSharedPointer<Maliit::Plugins::AbstractSurface>());
QSharedPointer<Maliit::Plugins::AbstractWidgetSurface> createTestWidgetSurface(const QSharedPointer<Maliit::Plugins::AbstractSurface> &parent = QSharedPointer<Maliit::Plugins::AbstractSurface>());


} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_TESTSURFACEFACTORY_H
