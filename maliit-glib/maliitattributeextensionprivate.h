/* This file is part of Maliit framework
 *
 * Copyright (C) 2012 One Laptop per Child Association
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

#ifndef MALIIT_GLIB_ATTRIBUTE_EXTENSION_PRIVATE_H
#define MALIIT_GLIB_ATTRIBUTE_EXTENSION_PRIVATE_H

#include <glib.h>

#define MALIIT_ATTRIBUTE_EXTENSION_DATA "maliit-attribute-extension"

#define MALIIT_ATTRIBUTE_EXTENSION_DATA_QUARK (g_quark_from_string (MALIIT_ATTRIBUTE_EXTENSION_DATA))

#endif /* MALIIT_GLIB_ATTRIBUTE_EXTENSION_PRIVATE_H */
