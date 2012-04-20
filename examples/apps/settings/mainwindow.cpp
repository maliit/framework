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

#include "mainwindow.h"

#include "pluginsettings.h"

#include <QStringList>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QtDebug>


MainWindow::MainWindow()
{
    QGridLayout *l = new QGridLayout(this);

    l->setColumnStretch(1, 1);
    l->addWidget(new QLabel("Layout"), 0, 0);
    l->addWidget(&language_selector, 0, 1);

    maliit_settings = Maliit::SettingsManager::create();
    maliit_settings->setParent(this);

    connect(maliit_settings, SIGNAL(pluginSettingsReceived(QList<QSharedPointer<Maliit::PluginSettings> >)),
            this, SLOT(pluginSettingsReceived(QList<QSharedPointer<Maliit::PluginSettings> >)));
    connect(maliit_settings, SIGNAL(connected()),
            this, SLOT(connected()));
    connect(&language_selector, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setLanguage(int)));
}

void MainWindow::connected()
{
    qDebug() << "SettingsWidget::connected()";

    maliit_settings->loadPluginSettings();
}

void MainWindow::pluginSettingsReceived(const QList<QSharedPointer<Maliit::PluginSettings> > &settings)
{
    qDebug() << "SettingsWidget::pluginSettingsReceived()";

    Q_FOREACH (const QSharedPointer<Maliit::PluginSettings> &setting, settings) {
	if (setting->pluginName() != "server")
	    continue;

        Q_FOREACH (const QSharedPointer<Maliit::SettingsEntry> &entry, setting->configurationEntries()) {
            if (entry->key() == "/maliit/onscreen/enabled") {
                // force-activate all layouts, for testing
                entry->set(entry->attributes()[Maliit::SettingEntryAttributes::valueDomain]);
            } else if (entry->key() == "/maliit/onscreen/active") {
                language_entry = entry;

                language_selector.addItem("Select language");
                QStringList values = entry->attributes()[Maliit::SettingEntryAttributes::valueDomain].toStringList();
                QStringList descriptions = entry->attributes()[Maliit::SettingEntryAttributes::valueDomainDescriptions].toStringList();
                for (int i = 0; i < values.count(); ++i) {
                    language_selector.addItem(descriptions[i], values[i]);
                }

                connect(language_entry.data(), SIGNAL(valueChanged()),
                        this, SLOT(languageChanged()));
            }
        }
    }

    language_selector.setCurrentIndex(language_selector.findData(language_entry->value()));
}

void MainWindow::setLanguage(int index)
{
    if (index == 0)
        return;

    qDebug() << "Setting layout" << language_selector.itemData(index);

    language_entry->set(language_selector.itemData(index));
}

void MainWindow::languageChanged()
{
    qDebug() << "New language is" << language_entry->value();

    language_selector.setCurrentIndex(language_selector.findData(language_entry->value()));
}
