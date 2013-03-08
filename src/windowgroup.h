/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2013 Openismus GmbH
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef MALIIT_SERVER_WINDOW_GROUP_H
#define MALIIT_SERVER_WINDOW_GROUP_H

#include <QObject>

#include <maliit/namespace.h>

class QWindow;

namespace Maliit
{

class WindowGroupPrivate;

class WindowGroup : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WindowGroup)

public:
    WindowGroup();
    ~WindowGroup();

    void activate();
    void deactivate();

    void setupWindow(QWindow *window, Maliit::Position position);

Q_SIGNALS:
    void inputMethodAreaChanged(const QRegion &inputMethodArea);

private:
    QScopedPointer<WindowGroupPrivate> d_ptr;

private Q_SLOTS:
  void onVisibleChanged(bool visible);
  void updateInputMethodArea();
};

} // namespace Maliit

#endif // MALIIT_SERVER_WINDOW_GROUP_H
