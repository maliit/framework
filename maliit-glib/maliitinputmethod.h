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

#ifndef MALIIT_GLIB_INPUT_METHOD_H
#define MALIIT_GLIB_INPUT_METHOD_H

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define MALIIT_TYPE_INPUT_METHOD           (maliit_input_method_get_type())
#define MALIIT_INPUT_METHOD(obj)           (G_TYPE_CHECK_INSTANCE_CAST(obj, MALIIT_TYPE_INPUT_METHOD, MaliitInputMethod))
#define MALIIT_INPUT_METHOD_CLASS(cls)     (G_TYPE_CHECK_CLASS_CAST(cls, MALIIT_TYPE_INPUT_METHOD, MaliitInputMethodClass))
#define MALIIT_IS_INPUT_METHOD(obj)        (G_TYPE_CHECK_INSTANCE_TYPE(obj, MALIIT_TYPE_INPUT_METHOD))
#define MALIIT_IS_INPUT_METHOD_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE(obj, MALIIT_TYPE_INPUT_METHOD))
#define MALIIT_INPUT_METHOD_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), MALIIT_TYPE_INPUT_METHOD, MaliitInputMethodClass))

typedef struct _MaliitInputMethod MaliitInputMethod;
typedef struct _MaliitInputMethodClass MaliitInputMethodClass;
typedef struct _MaliitInputMethodPrivate MaliitInputMethodPrivate;

struct _MaliitInputMethod
{
    GObject parent;

    MaliitInputMethodPrivate *priv;
};

/** MaliitInputMethodClass:
  * @parent_class: The parent class.
  **/
struct _MaliitInputMethodClass
{
    GObjectClass parent_class;
};

GType
maliit_input_method_get_type (void) G_GNUC_CONST;

MaliitInputMethod *
maliit_input_method_new (void);

void
maliit_input_method_get_area (MaliitInputMethod *input_method,
                              int *x, int *y,
                              int *width, int *height);

void
maliit_input_method_show (MaliitInputMethod *input_method);

void
maliit_input_method_hide (MaliitInputMethod *input_method);

G_END_DECLS

#endif /* MALIIT_GLIB_INPUT_METHOD_H */
