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
#ifndef MIMSETTINGDIALOG_H
#define MIMSETTINGDIALOG_H

#include <QObject>
#include <QList>
#include <QStringList>
#include <QMap>
#include <QTimer>
#include <QSet>
#include <QRegion>
#include <MDialog>

class MIMPluginManagerPrivate;
class MContentItem;
class MPopupList;
class QModelIndex;
class MContainer;
class MInputMethodSettingsBase;

class MIMSettingsDialog : public MDialog
{
    Q_OBJECT
public:
    MIMSettingsDialog(MIMPluginManagerPrivate *,
                     const QString &title, M::StandardButtons buttons);

    virtual ~MIMSettingsDialog();

Q_SIGNALS:
    void languageChanged();

protected:
    //! reimp
    virtual void retranslateUi();
    virtual void orientationChangeEvent(MOrientationChangeEvent *event);
    //! reimp_end

private slots:
    void retranslateSettingsUi();

private:

    QMap<MInputMethodSettingsBase *, MContainer *> settingsContainerMap;
    MIMPluginManagerPrivate *const imPluginManagerPrivate;
};

#endif
