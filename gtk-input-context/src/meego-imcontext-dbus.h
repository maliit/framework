/*
 * Copyright (C) 2010 Intel Corporation
 *
 * Author:  Raymond Liu <raymond.liu@intel.com>
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
 *
 */

#ifndef _MEEGO_IMCONTEXT_DBUS_H
#define _MEEGO_IMCONTEXT_DBUS_H

#include <glib.h>
#include <glib-object.h>

typedef struct {
    GObject parent;

} MeegoIMContextDbusObj;

typedef struct {
    GObjectClass parent;
} MeegoIMContextDbusObjClass;

GType meego_imcontext_dbusobj_get_type(void);

#define MEEGO_IMCONTEXT_TYPE_DBUSOBJ (meego_imcontext_dbusobj_get_type())
#define MEEGO_IMCONTEXT_DBUSOBJ(obj) \
        (G_TYPE_CHECK_INSTANCE_CAST((obj), MEEGO_IMCONTEXT_TYPE_DBUSOBJ, MeegoIMContextDbusObj))
#define MEEGO_IMCONTEXT_DBUSOBJ_CLASS(klass) \
        (G_TYPE_CHECK_CLASS_CAST((klass), MEEGO_IMCONTEXT_TYPE_DBUSOBJ, MeegoIMContextDbusObjClass))
#define MEEGO_IMCONTEXT_IS_DBUSOBJ(obj) \
        (G_TYPE_CHECK_INSTANCE_TYPE((obj), MEEGO_IMCONTEXT_TYPE_DBUSOBJ))
#define MEEGO_IMCONTEXT_IS_DBUSOBJ_CLASS(klass) \
        (G_TYPE_CHECK_CLASS_TYPE((klass), MEEGO_IMCONTEXT_TYPE_DBUSOBJ))
#define MEEGO_IMCONTEXT_DBUSOBJ_GET_CLASS(obj) \
        (G_TYPE_INSTANCE_GET_CLASS((obj), MEEGO_IMCONTEXT_TYPE_DBUSOBJ, MeegoIMContextDbusObjClass))


MeegoIMContextDbusObj *meego_imcontext_dbusobj_get_singleton(void);
void meego_imcontext_dbusobj_connect(MeegoIMContextDbusObj *obj, gpointer connection);

#endif /* _MEEGO_IMCONTEXT_DBUS_H */
