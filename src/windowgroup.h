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
#include <QSharedPointer>
#include <QTimer>
#include <QRegion>
#include <QVector>
#include <QWindow>

#include "windowdata.h"

#include <maliit/namespace.h>

class QWindow;

namespace Maliit
{

class AbstractPlatform;
class WindowGroupPrivate;

class WindowGroup : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WindowGroup)

public:
    enum HideMode {
        HideImmediate,
        HideDelayed
    };

    WindowGroup(const QSharedPointer<AbstractPlatform> &platform);
    ~WindowGroup();

    void activate();
    void deactivate(HideMode mode);

    void setupWindow(QWindow *window, Maliit::Position position);
    void setScreenRegion(const QRegion &region, QWindow *window);
    void setInputMethodArea(const QRegion &region, QWindow *window);
    void setApplicationWindow(WId id);

Q_SIGNALS:
    void inputMethodAreaChanged(const QRegion &inputMethodArea);

private Q_SLOTS:
    void hideWindows();
    void onVisibleChanged(bool visible);
    void updateInputMethodArea();

private:
    bool containsWindow(QWindow *window);

    QSharedPointer<AbstractPlatform> m_platform;
    QVector<WindowData> m_window_list;
    QRegion m_last_im_area;
    bool m_active;
    QTimer m_hideTimer;
};

} // namespace Maliit

#endif // MALIIT_SERVER_WINDOW_GROUP_H
