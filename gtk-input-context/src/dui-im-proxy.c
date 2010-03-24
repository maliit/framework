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

#include "dui-im-proxy.h"

G_DEFINE_TYPE (DuiIMProxy, dui_im_proxy, G_TYPE_OBJECT);

static void dui_im_proxy_finalize(GObject *object);
static void dui_im_proxy_class_init (DuiIMProxyClass *klass);
static void dui_im_proxy_init (DuiIMProxy *dui_im_proxy);


DuiIMProxy *
dui_im_proxy_get_singleton (void)
{
	static DuiIMProxy *proxy = NULL;
	if (!proxy)
		proxy = g_object_new(DUI_TYPE_IM_PROXY, NULL);
	return proxy;
}


static void
dui_im_proxy_finalize(GObject *object)
{
	//DuiIMProxy *im_proxy = DUI_IM_PROXY(object);

	G_OBJECT_CLASS(dui_im_proxy_parent_class)->finalize(object);
}


static void
dui_im_proxy_class_init (DuiIMProxyClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = dui_im_proxy_finalize;
}


static void
dui_im_proxy_init (DuiIMProxy *dui_im_proxy)
{


}

