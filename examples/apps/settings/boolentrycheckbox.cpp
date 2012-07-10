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

#include "boolentrycheckbox.h"

BoolEntryCheckBox::BoolEntryCheckBox(const QSharedPointer<Maliit::SettingsEntry>& entry)
    : QCheckBox()
    , m_entry(entry)
{
    if (m_entry) {
        const bool value(m_entry->value().toBool());

        // we are setting the checked value and text manually without
        // relying on signal emission, because signal is not emitted
        // when there is no actual change of state.
        setChecked(value);
        setText(value ? "True" : "False");
        connect(this, SIGNAL(toggled(bool)),
                this, SLOT(onToggled(bool)));
    } else {
        setDisabled(true);
    }
}

void BoolEntryCheckBox::onToggled(bool checked)
{
    if (m_entry) {
        if (checked) {
            setText("True");
            m_entry->set(true);
        } else {
            setText("False");
            m_entry->set(false);
        }
    }
}
