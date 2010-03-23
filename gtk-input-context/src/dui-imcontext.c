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

#include "dui-imcontext.h"

static GType _dui_imcontext_type = 0;
static GtkIMContextClass *parent_class = NULL;

static void dui_imcontext_finalize(GObject *object);
static void dui_imcontext_class_init (DuiIMContextClass *klass);
static void dui_imcontext_init (DuiIMContext *dui_imcontext);


void
dui_imcontext_register_type (GTypeModule *type_module)
{
	static const GTypeInfo dui_imcontext_info = {
		sizeof (DuiIMContextClass),
		(GBaseInitFunc) NULL,
		(GBaseFinalizeFunc) NULL,
		(GClassInitFunc) dui_imcontext_class_init,
		NULL,
		NULL,
		sizeof (DuiIMContext),
		0,
		(GInstanceInitFunc) dui_imcontext_init,
	};

	if (_dui_imcontext_type)
		return;

        if (type_module) {
		_dui_imcontext_type = 
			g_type_module_register_type(
				type_module,
				GTK_TYPE_IM_CONTEXT,
				"DuiIMContext",
				&dui_imcontext_info,
				(GTypeFlags)0);
        } else {
		_dui_imcontext_type =
			g_type_register_static(
				GTK_TYPE_IM_CONTEXT,
				"DuiIMContext",
				&dui_imcontext_info,
				(GTypeFlags)0);
        }
}


GtkIMContext *
dui_imcontext_new (void)
{
	DuiIMContext *ic = DUI_IMCONTEXT(g_object_new(DUI_TYPE_IMCONTEXT, NULL));
	return GTK_IM_CONTEXT(ic);
}


static void
dui_imcontext_finalize(GObject *object)
{
	//DuiIMContext *imcontext = DUI_IMCONTEXT(object);

	G_OBJECT_CLASS(parent_class)->finalize(object);
}


static void
dui_imcontext_class_init (DuiIMContextClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	parent_class = (GtkIMContextClass *)g_type_class_peek_parent(klass);

	gobject_class->finalize = dui_imcontext_finalize;

}


static void
dui_imcontext_init (DuiIMContext *dui_imcontext)
{


}

