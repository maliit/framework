/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2012 Mattia Barbon <mattia@develer.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "settingsmanager.h"
#include "settingsentry.h"

#include <QWidget>
#include <QComboBox>


class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow();

private Q_SLOTS:
    void pluginSettingsReceived(const QList<QSharedPointer<Maliit::PluginSettings> > &settings);
    void connected();

    void setLanguage(int index);
    void languageChanged();

private:
    Maliit::SettingsManager *maliit_settings;
    QSharedPointer<Maliit::SettingsEntry> language_entry;
    QComboBox language_selector;
};

#endif
