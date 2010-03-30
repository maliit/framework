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
 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <gtk/gtkimmodule.h>

#include "dui-imcontext.h"
#include "debug.h"

static const GtkIMContextInfo dui_im_info =
{
	"dui-im",
	"DUI Input Method",
	"dui-im",
	"",
	"ja:ko:zh:*"
};


static const GtkIMContextInfo *info_list[] =
{
	&dui_im_info
};


void im_module_init (GTypeModule *type_module);
void im_module_exit (void);
void im_module_list (const GtkIMContextInfo ***contexts, int *context_number);
GtkIMContext *im_module_create (const gchar *context_id);


void
im_module_init (GTypeModule *type_module)
{
	STEP();
	g_type_module_use(type_module);
	dui_imcontext_register_type(type_module);
	STEP();
}


void
im_module_exit ()
{
	STEP();
}


void
im_module_list (const GtkIMContextInfo ***contexts, int *context_number)
{
	*contexts = info_list;
	*context_number = G_N_ELEMENTS(info_list);
}


GtkIMContext *
im_module_create (const gchar *context_id)
{
	if (g_strcmp0(context_id, "dui-im") == 0) {
		return dui_imcontext_new();
	} else {
		return NULL;
	}
}

