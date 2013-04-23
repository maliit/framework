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

#ifndef MIMSERVER_H
#define MIMSERVER_H

#include <QObject>
#include <QSharedPointer>

class MInputContextConnection;
class MImServerPrivate;


namespace Maliit
{

class AbstractPlatform;

} // namespace Maliit

/* MImServer: The Maliit Input Method Server
 *
 * Consumers of MImServer are responsible for creating a QApplication (for the mainloop),
 * and an MInputContextConnection for communication with clients, and for starting the mainloop.
 * Everything else is handled by the server.
 */
class MImServer : public QObject
{
    Q_OBJECT

public:
    enum SettingsType {
        TemporarySettings,
        PersistentSettings
    };

public:
    explicit MImServer(const QSharedPointer<MInputContextConnection> &icConnection,
                       const QSharedPointer<Maliit::AbstractPlatform> &platform,
                       QObject *parent = 0);
    ~MImServer();

    static void configureSettings(MImServer::SettingsType settingsType);

private:
    Q_DISABLE_COPY(MImServer)
    Q_DECLARE_PRIVATE(MImServer)

    const QScopedPointer<MImServerPrivate> d_ptr;
};

#endif // MIMSERVER_H
