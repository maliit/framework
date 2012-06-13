/* This file is part of Maliit framework
 *
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

#include "selectentrycombobox.h"

SelectEntryComboBox::SelectEntryComboBox(const QSharedPointer<Maliit::SettingsEntry>& entry)
    : m_entry(entry)
{
    if (m_entry) {
        const QStringList values(entry->attributes()[Maliit::SettingEntryAttributes::valueDomain].toStringList());
        const QStringList descriptions(entry->attributes()[Maliit::SettingEntryAttributes::valueDomainDescriptions].toStringList());

        for (int index(0); index < values.count(); ++index) {
            addItem(descriptions[index], values[index]);
        }

        onValueChanged();

        connect(this, SIGNAL(currentIndexChanged(int)),
                this, SLOT(onSelected(int)));

        connect(m_entry.data(), SIGNAL(valueChanged()),
                this, SLOT(onValueChanged()));
    } else {
        setDisabled(true);
    }
}

void SelectEntryComboBox::onSelected(int index)
{
    if (m_entry) {
        m_entry->set(itemData(index));
    }
}

void SelectEntryComboBox::onValueChanged()
{
    if (m_entry) {
        setCurrentIndex(findData(m_entry->value()));
    }
}
