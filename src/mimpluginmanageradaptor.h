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

#ifndef MIMPLUGINMANAGERADAPTOR_H
#define MIMPLUGINMANAGERADAPTOR_H

#include <QObject>
#include <QDBusAbstractAdaptor>
#include <QString>
#include <QStringList>
#include <QMap>


class MIMPluginManager;

//! \internal
class MIMPluginManagerAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.meego.inputmethodpluginmanager1")

public:
    explicit MIMPluginManagerAdaptor(MIMPluginManager *parent);
    virtual ~MIMPluginManagerAdaptor();

public slots:
    QStringList queryAvailablePlugins();
    QStringList queryAvailablePlugins(int state);
    QString queryActivePlugin(int state);

    //! Returns all available subviews (IDs and titles).
    QMap<QString, QVariant> queryAvailableSubViews(const QString &pluginName, int state);

    //! Returns the active subview ID and the plugin which it belongs.
    QMap<QString, QVariant> queryActiveSubView(int state);
    Q_NOREPLY void setActivePlugin(const QString &pluginname, int state,
                                   const QString &subviewId = QString());
    Q_NOREPLY void setActiveSubView(const QString &subViewId, int state);

signals:
    //! This signal is emitted when the active subview of \a state is changed.
    void activeSubViewChanged(int state);

private:
    MIMPluginManager *owner;
    friend class MIMPluginManager;
    friend class MIMPluginManagerPrivate;
};
//! \internal_end

#endif
