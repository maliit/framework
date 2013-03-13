/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MIMPLUGINSPROXYWIDGET_H
#define MIMPLUGINSPROXYWIDGET_H

#include <QWidget>

/*! \ingroup maliitserver
 * \brief Proxy QWidget for input method plugins.
 *
 * Used to manage the visibility of plugins without having to infer that from
 * the plugin's own visibility.
 */
class MImPluginsProxyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MImPluginsProxyWidget(QWidget *parent = 0);
};

#endif // MIMPLUGINSPROXYWIDGET_H
