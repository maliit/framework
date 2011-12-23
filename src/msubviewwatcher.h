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

#ifndef MSUBVIEWWATCHER_H
#define MSUBVIEWWATCHER_H

#include "mimonscreenplugins.h"
#include "minputcontextconnection.h"

#include <QObject>
#include <QWeakPointer>
#include <QSet>

//! \internal
/*! \ingroup maliitserver
 * \brief This class watche on subview usage and records which subviews were really used.
 * \note this class is not considered stable API
 */
class MSubViewWatcher : public QObject
{
Q_OBJECT

public:
    /*!
     * \brief Create new instance.
     * \param newConnection Connection to input context
     * \param newOnScreenPlugins Object providing (and managing) status of subviews
     */
    MSubViewWatcher(MInputContextConnection *newConnection,
                    MImOnScreenPlugins * newOnScreenPlugins,
                    QObject *parent = 0);

    //! \brief Destroy instance and enable recorded plugins
    virtual ~MSubViewWatcher();

private Q_SLOTS:
    //! \brief Start watching on user activity
    void startWatching();

    //! \brief Remember current subview as required one
    void recordActiveSubView();

private:
    //! \brief Modify settings to enable all required subviews
    void enableRecordedSubViews();

    QWeakPointer<MInputContextConnection> connection;
    QWeakPointer<MImOnScreenPlugins> onScreenPlugins;
    QSet<MImOnScreenPlugins::SubView> recordedSubViews;
    bool connected; //!< Contains true if startWatching() is already connected to signal

    friend class Ut_MSubViewWatcher;
};
//! \internal_end

#endif
