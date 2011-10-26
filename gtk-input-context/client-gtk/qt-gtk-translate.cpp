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

#include "debug.h"
#include <QEvent>
#include <QKeyEvent>

#include "qt-keysym-map.h"

// To workaround glib >= 2.25 where signals is used, while QT already define it as C++ keyword: protected
#ifdef signals
#undef signals
#endif
#include "qt-gtk-translate.h"


GdkEventKey *
compose_gdk_keyevent(GdkEventType type, guint keyval, guint state, GdkWindow *window)
{
	GdkEventKey *event = NULL;
#if GTK_CHECK_VERSION (3, 0, 0)
	GdkDisplay *display;
	GdkDeviceManager *device_manager;
	GdkDevice *client_pointer;
#endif
	if ((type != GDK_KEY_PRESS) && (type != GDK_KEY_RELEASE))
		return NULL;

	event = (GdkEventKey *)(gdk_event_new(type));
	event->length = 0;
	event->string = 0;
	event->is_modifier = 0;
	event->time = GDK_CURRENT_TIME;
	event->state = state;

#if GTK_CHECK_VERSION (3, 0, 0)
	display = gdk_window_get_display (window);
	device_manager = gdk_display_get_device_manager (display);
	client_pointer = gdk_device_manager_get_client_pointer (device_manager);

	gdk_event_set_device ((GdkEvent *)event,
			      gdk_device_get_associated_device (client_pointer));
#endif

	if (type == GDK_KEY_RELEASE)
		event->state |= GDK_RELEASE_MASK;
	event->keyval = keyval;
	event->window = window;

	if (event->window) {
		GdkKeymap *key_map = gdk_keymap_get_default();
		GdkKeymapKey *keys;
		gint n;

		g_object_ref(event->window); // seems when event is freed, the event->window will be unref

		if (gdk_keymap_get_entries_for_keyval(key_map, event->keyval, &keys, &n)) {
			event->hardware_keycode = keys[0].keycode;
			event->group = keys[0].group;
		} else {
			event->hardware_keycode = 0;
			event->group = 0;
		}
	}

	DBG("event type=0x%x, state=0x%x, keyval=0x%x, keycode=0x%x, group=%d",
		event->type, event->state, event->keyval, event->hardware_keycode, event->group);

	return event;
}



GdkEventKey *
qt_key_event_to_gdk(int type, int key, int modifiers, char *text, GdkWindow *window)
{
	guint state = 0;
	guint keyval;

	STEP();
	if ((type != QEvent::KeyPress) && (type != QEvent::KeyRelease))
		return NULL;

	if (modifiers & Qt::ShiftModifier)
		state |= GDK_SHIFT_MASK;
	if (modifiers & Qt::ControlModifier)
		state |= GDK_CONTROL_MASK;
	if (modifiers & Qt::AltModifier)
		state |= GDK_MOD1_MASK;

	keyval = QtKeyToXKeySym(key);

	if (type == QEvent::KeyPress) {
		return compose_gdk_keyevent(GDK_KEY_PRESS, keyval, state, window);
	} else {
		return compose_gdk_keyevent(GDK_KEY_RELEASE, keyval, state, window);
	}
	
}


gboolean
gdk_key_event_to_qt(GdkEventKey *event, int *type, int *key, int *modifier)
{

	switch (event->type) {
	case GDK_KEY_PRESS:
		*type = QEvent::KeyPress;
		break;
	case GDK_KEY_RELEASE:
		*type = QEvent::KeyRelease;
		break;
	default:
		return FALSE;
	}

	*key = XKeySymToQTKey(event->keyval);
	if (*key == Qt::Key_unknown) {
		qWarning("Unkonwn key");
		return FALSE;
	}

	*modifier = Qt::NoModifier;
	if (event->state & GDK_SHIFT_MASK)
		*modifier |= Qt::ShiftModifier;
	if (event->state & GDK_CONTROL_MASK)
		*modifier |= Qt::ControlModifier;
	if (event->state & GDK_MOD1_MASK)
		*modifier |= Qt::AltModifier;
	if (event->state & GDK_META_MASK)
		*modifier |= Qt::MetaModifier;

	DBG("qtkey type =%d, qtkey=0x%x, modifier=0x%x", *type, *key, *modifier);

	return TRUE;
}
