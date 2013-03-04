/* This file is part of Maliit framework
 *
 * Copyright (C) 2012 Mattia Barbon <mattia@develer.com>
 * Copyright (C) 2012 Canonical Ltd
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

#include "mainwindow.h"
#include "pluginsettings.h"
#include "stringentryedit.h"
#include "selectentrycombobox.h"
#include "boolentrycheckbox.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtGui/QInputMethod>
#endif

namespace
{

void
removeAllTabs (QTabWidget& tabs)
{
    for (int iter(tabs.count() - 1); iter >= 0; ++iter) {
        tabs.removeTab(iter);
    }
}

}

MainWindow::MainWindow() :
    maliit_settings(Maliit::SettingsManager::create(this)),
    language_entry(),
    enabled_entry(),
    language_selector(0),
    enable_all(0),
    im_testing_entry(new QTextEdit),
    tabs()
{
    QVBoxLayout *l = new QVBoxLayout(this);
    l->addWidget(new QLabel("Text entry for testing"));

    im_testing_entry->setFocusPolicy(Qt::StrongFocus);
    im_testing_entry->installEventFilter(this);
    l->addWidget(im_testing_entry);

    // Steals focus from im_testing_entry and therefore dismisses input
    // method. Other widgets should set im_testing_entry as focus proxy.
    QPushButton *dismiss_im = new QPushButton("Dismiss input method");
    l->addWidget(dismiss_im);
    l->addWidget(&tabs);

    connect(maliit_settings, SIGNAL(pluginSettingsReceived(QList<QSharedPointer<Maliit::PluginSettings> >)),
            this, SLOT(pluginSettingsReceived(QList<QSharedPointer<Maliit::PluginSettings> >)));
    connect(maliit_settings, SIGNAL(connected()),
            this, SLOT(connected()));
}

bool MainWindow::eventFilter(QObject *watched,
                             QEvent *event)
{
    // Let the input method show up on focus-in, not on second click:
    if (watched == im_testing_entry
        && event->type() == QFocusEvent::FocusIn) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        qApp->inputMethod()->show();
#else
        if (QInputContext *ic = qApp->inputContext()) {
            QEvent im_request(QEvent::RequestSoftwareInputPanel);
            ic->filterEvent(&im_request);
        }
#endif
    }

    return false;
}

void MainWindow::connected()
{
    qDebug() << "SettingsWidget::connected()";

    maliit_settings->loadPluginSettings();
}

void MainWindow::pluginSettingsReceived(const QList<QSharedPointer<Maliit::PluginSettings> > &settings)
{
    qDebug() << "SettingsWidget::pluginSettingsReceived()";

    const int current_index(tabs.count() > 0 ? tabs.currentIndex() : 0);

    if (language_selector) {
        disconnect(language_selector, SIGNAL(currentIndexChanged(int)),
                   this, SLOT(setLanguage(int)));
        language_selector = 0;
    }
    if (enable_all) {
        disconnect(enable_all, SIGNAL(clicked()),
                   this, SLOT(enableAllLayouts()));
        enable_all = 0;
    }
    removeAllTabs(tabs);

    Q_FOREACH (const QSharedPointer<Maliit::PluginSettings> &setting, settings) {
        const QString plugin_name(setting->pluginName());
        QWidget *page(new QWidget);
        QGridLayout *layout(new QGridLayout(page));

        int row = 0;

        if (plugin_name == "server") {
            language_selector = new QComboBox;
            language_selector->setFocusProxy(im_testing_entry);

            enable_all = new QPushButton("Enable all keyboard layouts");
            enable_all->setFocusProxy(im_testing_entry);

            Q_FOREACH (const QSharedPointer<Maliit::SettingsEntry> &entry, setting->configurationEntries()) {
                if (not entry) {
                    continue;
                }

                if (entry->key() == "/maliit/onscreen/enabled") {
                    enabled_entry = entry;
                } else if (entry->key() == "/maliit/onscreen/active") {
                    QStringList values = entry->attributes()[Maliit::SettingEntryAttributes::valueDomain].toStringList();
                    QStringList descriptions = entry->attributes()[Maliit::SettingEntryAttributes::valueDomainDescriptions].toStringList();

                    language_entry = entry;
                    language_selector->addItem("Select language");
                    for (int i = 0; i < values.count(); ++i) {
                        language_selector->addItem(descriptions[i], values[i]);
                    }

                    connect(language_entry.data(), SIGNAL(valueChanged()),
                            this, SLOT(languageChanged()));
                }
            }

            connect(language_selector, SIGNAL(currentIndexChanged(int)),
                    this, SLOT(setLanguage(int)));
            connect(enable_all, SIGNAL(clicked()),
                    this, SLOT(enableAllLayouts()));

            layout->setColumnStretch(1, 1);
            layout->addWidget(new QLabel("Layout"), row, 0);
            layout->addWidget(language_selector, row, 1);
            ++row;
            layout->addWidget(enable_all, row, 1);
            ++row;

            language_selector->setCurrentIndex(language_selector->findData(language_entry->value()));
            tabs.insertTab(0, page, plugin_name);
        } else {
            typedef QPair<QWidget*, QWidget*> WPair;
            typedef QList<WPair> WPairList;

            WPairList wlist;
            wlist.append(WPair(new QLabel("Plugin desc:"), new QLabel(setting->pluginDescription())));

            Q_FOREACH (const QSharedPointer<Maliit::SettingsEntry> &entry, setting->configurationEntries()) {
                if (not entry) {
                    continue;
                }

                wlist.append(WPair(new QLabel("Key:"), new QLabel(entry->key())));
                wlist.append(WPair(new QLabel("Entry desc:"), new QLabel(entry->description())));

                QWidget *second_widget(0);
                const QVariant value(entry->value());

                switch (entry->type()) {
                case Maliit::StringType: {
                    const QStringList values = entry->attributes()[Maliit::SettingEntryAttributes::valueDomain].toStringList();

                    if (values.isEmpty()) {
                        second_widget = new StringEntryEdit(entry);
                    } else {
                        second_widget = new SelectEntryComboBox(entry);
                    }
                } break;
                case Maliit::IntType:
                    second_widget = new QLabel(QString::number(value.toInt()));
                    break;
                case Maliit::BoolType:
                    second_widget = new BoolEntryCheckBox(entry);
                    break;
                case Maliit::StringListType:
                    second_widget = new QLabel(value.toStringList().join(", "));
                    break;
                case Maliit::IntListType: {
                    const QVariantList values(value.toList());
                    QStringList strings;

                    Q_FOREACH (const QVariant &v, values) {
                        strings << QString::number(v.toInt());
                    }

                    second_widget = new QLabel(strings.join(", "));
                } break;

                }

                wlist.append(WPair(new QLabel("Value:"), second_widget));
                second_widget->setFocusProxy(im_testing_entry);

                Q_FOREACH(const WPair& pair, wlist) {
                    layout->addWidget(pair.first, row, 0);
                    layout->addWidget(pair.second, row, 1);
                    ++row;
                }
            }
            tabs.addTab(page, plugin_name);
        }

        layout->setRowStretch(row, 10);
    }

    const int tabs_count(tabs.count());

    if (tabs_count > 0) {
        if (tabs_count > current_index) {
            tabs.setCurrentIndex(current_index);
        } else {
            tabs.setCurrentIndex(0);
        }
    }
}

void MainWindow::setLanguage(int index)
{
    if (index == 0 or not language_selector) {
        return;
    }

    qDebug() << "Setting layout" << language_selector->itemData(index);

    if (language_entry && enabled_entry) {
        const QString layout = language_selector->itemData(index).toString();

        // Make sure the new layout is enabled
        QStringList list = enabled_entry->value().toStringList();
        if (!list.contains(layout)) {
            qDebug() << "Enabling layout first";
            list.append(layout);
            enabled_entry->set(list);
        }

        // Now activate the new layout
        language_entry->set(layout);
    } else {
        qCritical() << "Language entry is NULL";
    }
}

void MainWindow::enableAllLayouts()
{
    if (enabled_entry) {
        enabled_entry->set(enabled_entry->attributes()[Maliit::SettingEntryAttributes::valueDomain]);
    } else {
        qCritical() << "Enabled entry is NULL";
    }
}

void MainWindow::languageChanged()
{
    if (not language_selector) {
        return;
    }

    if (language_entry) {
        qDebug() << "New language is" << language_entry->value();

        language_selector->setCurrentIndex(language_selector->findData(language_entry->value()));
    } else {
        qDebug() << "No language is selected";
    }
}
