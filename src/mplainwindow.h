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
#ifndef MPLAINWINDOW_H
#define MPLAINWINDOW_H

#include <MWindow>

class MScene;
class MSceneManager;
class QRegion;

class MPlainWindow : public MWindow
{
    Q_OBJECT

public:
    MPlainWindow(QWidget *parent = 0);
    virtual ~MPlainWindow();
    static MPlainWindow *instance();

public slots:
#if defined(M_IM_DISABLE_TRANSLUCENCY) && !defined(M_IM_USE_SHAPE_WINDOW)
    void updatePosition(const QRegion &region);
#endif

private:
    Q_DISABLE_COPY(MPlainWindow);

    static MPlainWindow *m_instance;
};

#endif // MPLAINWINDOW_H
