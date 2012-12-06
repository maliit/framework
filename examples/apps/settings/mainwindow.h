/* This file is part of Maliit framework
 *
 * Copyright (C) 2012 Mattia Barbon <mattia@develer.com>
 * Copyright (C) 2012 Openismus GmbH
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "settingsmanager.h"
#include "settingsentry.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QTextEdit>
#endif

#include <QtGui>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow();

protected:
    bool eventFilter(QObject *watched,
                     QEvent *event);

private Q_SLOTS:
    void pluginSettingsReceived(const QList<QSharedPointer<Maliit::PluginSettings> > &settings);
    void connected();

    void setLanguage(int index);
    void languageChanged();
    void enableAllLayouts();

private:
    Maliit::SettingsManager *maliit_settings;
    QSharedPointer<Maliit::SettingsEntry> language_entry, enabled_entry;
    QComboBox* language_selector;
    QPushButton* enable_all;
    QTextEdit *im_testing_entry;
    QTabWidget tabs;
};

#endif
