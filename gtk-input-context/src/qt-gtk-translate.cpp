/*
 * Copyright (C) 2010, Intel Corporation.
 *
 * Author: Raymond Liu <raymond.liu@intel.com>
 * 
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "qt-gtk-translate.h"
#include "debug.h"
#include <QEvent>
#include <QKeyEvent>

GdkEventKey *
qt_key_event_to_gdk(int type, int key, int modifiers)
{
	GdkEventKey *event = NULL;
	STEP();
	if ((type != QEvent::KeyPress) && (type != QEvent::KeyRelease))
		return NULL;

	if (type == QEvent::KeyPress) {
		DBG("%x: key pressed with modifier %x", key, modifiers);
	} else {
		DBG("%x: key released with modifier %x", key, modifiers);
	}

	return event;
}

