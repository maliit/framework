/* * This file is part of maliit-framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef MDBUSGLIBINPUTCONTEXTADAPTOR_H
#define MDBUSGLIBINPUTCONTEXTADAPTOR_H

#include <glib-object.h>

class MImServerConnection;

//! \brief Glib D-Bus version of an adaptor that exposes MInputContext interface
//! through D-Bus
struct MDBusGlibInputContextAdaptor
{
    GObject parent;
    MImServerConnection *imServerConnection;
};

//! \brief MDBusGlibInputContextAdaptor metaclass
struct MDBusGlibInputContextAdaptorClass
{
    GObjectClass parent;
};

#define M_TYPE_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR              (m_dbus_glib_input_context_adaptor_get_type())
#define M_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR(object)           (G_TYPE_CHECK_INSTANCE_CAST((object), M_TYPE_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR, MDBusGlibInputContextAdaptor))
#define M_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), M_TYPE_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR, MDBusGlibInputContextAdaptorClass))
#define M_IS_M_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR(object)      (G_TYPE_CHECK_INSTANCE_TYPE((object), M_TYPE_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR))
#define M_IS_M_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), M_TYPE_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR))
#define M_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), M_TYPE_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR, MDBusGlibInputContextAdaptorClass))


GType m_dbus_glib_input_context_adaptor_get_type(void);


#endif
