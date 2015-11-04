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

#include "maliitinputmethod.h"
#include "maliitmarshallers.h"
#include "meego-im-connector.h"

/**
 * SECTION:maliitinputmethod
 * @short_description: input method handling
 * @title: MaliitInputMethod
 * @stability: Stable
 * @include: maliit/maliitinputmethod.h
 *
 * #MaliitInputMethod class can be used by application to query
 * maliit-server for currently shown IM plugin area and to request
 * maliit-server to show or hide the IM plugin.
 */

struct _MaliitInputMethodPrivate
{
    int area[4];

    MeegoIMProxy *maliit_proxy;
};

G_DEFINE_TYPE (MaliitInputMethod, maliit_input_method, G_TYPE_OBJECT)

enum
{
    AREA_CHANGED,

    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void
maliit_input_method_finalize (GObject *object)
{
    G_OBJECT_CLASS (maliit_input_method_parent_class)->finalize (object);
}

static void
maliit_input_method_dispose (GObject *object)
{
    MaliitInputMethod *input_method = MALIIT_INPUT_METHOD (object);
    MaliitInputMethodPrivate *priv = input_method->priv;

    priv->maliit_proxy = NULL;

    G_OBJECT_CLASS (maliit_input_method_parent_class)->dispose (object);
}

static void
maliit_input_method_class_init (MaliitInputMethodClass *input_method_class)
{
    GObjectClass *g_object_class = G_OBJECT_CLASS (input_method_class);

    g_object_class->finalize = maliit_input_method_finalize;
    g_object_class->dispose = maliit_input_method_dispose;

    /**
     * MaliitInputMethod::area-changed:
     * @input_method: The #MaliitInputMethod emitting the signal.
     * @x: X coordinate of new input method area's top-left corner.
     * @y: Y coordinate of new input method area's top-left corner.
     *.
     * @width: Width of new input method area.
     * @height: Height of new input method area.
     *
     * Informs application that the input method area (area on the screen
     * occupied by a virtual keyboard) is changed.
     */
    signals[AREA_CHANGED] =
        g_signal_new ("area-changed",
                      MALIIT_TYPE_INPUT_METHOD,
                      G_SIGNAL_RUN_FIRST,
                      0,
                      NULL,
                      NULL,
                      maliit_marshal_VOID__INT_INT_INT_INT,
                      G_TYPE_NONE,
                      4,
                      G_TYPE_INT,
                      G_TYPE_INT,
                      G_TYPE_INT,
                      G_TYPE_INT);

    g_type_class_add_private (input_method_class, sizeof (MaliitInputMethodPrivate));
}

static void
update_input_method_area(MaliitInputMethod *input_method,
                         int x, int y, int width, int height,
                         gpointer user_data G_GNUC_UNUSED)
{
    input_method->priv->area[0] = x;
    input_method->priv->area[1] = y;
    input_method->priv->area[2] = width;
    input_method->priv->area[3] = height;

    g_signal_emit(input_method, signals[AREA_CHANGED], 0, x, y, width, height);
}

static void
maliit_input_method_init (MaliitInputMethod *input_method)
{
    MaliitInputMethodPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (input_method,
                                                                  MALIIT_TYPE_INPUT_METHOD,
                                                                  MaliitInputMethodPrivate);
    MeegoImConnector *connection = meego_im_connector_get_singleton();

    priv->area[0] = priv->area[1] = 0;
    priv->area[2] = priv->area[3] = 0;

    priv->maliit_proxy = connection->proxy;

    input_method->priv = priv;

    g_signal_connect_swapped(connection->dbusobj, "update-input-method-area",
                             G_CALLBACK(update_input_method_area), input_method);
}

/**
 * maliit_input_method_new:
 *
 * Returns a new maliit input method.
 *
 * Returns: (transfer full): A new #MaliitInputMethod.
 */
MaliitInputMethod *
maliit_input_method_new (void)
{
    return MALIIT_INPUT_METHOD (g_object_new (MALIIT_TYPE_INPUT_METHOD,
                                              NULL));
}

/**
 * maliit_input_method_get_area:
 * @input_method: (transfer none): The #MaliitInputMethod which input method area you want to get.
 * @x: (out): X coordinate of current input method area's top-left corner.
 * @y: (out): Y coordinate of current input method area's top-left corner.
 * @width: (out): Width of current input method area.
 * @height: (out): Height of current input method area.
 *
 * Get the current input method area in @x, @y, @width and @height.
 */
void
maliit_input_method_get_area (MaliitInputMethod *input_method,
                              int *x, int *y,
                              int *width, int *height)
{
    g_return_if_fail (MALIIT_IS_INPUT_METHOD (input_method));

    if (x)
        *x = input_method->priv->area[0];
    if (y)
        *y = input_method->priv->area[1];
    if (width)
        *width = input_method->priv->area[2];
    if (height)
        *height = input_method->priv->area[3];
}

/**
 * maliit_input_method_show:
 * @input_method: (transfer none): The #MaliitInputMethod which you want to show.
 *
 * Request to explicitly show the Maliit virtual keyboard.
 */
void
maliit_input_method_show (MaliitInputMethod *input_method)
{
    g_return_if_fail (MALIIT_IS_INPUT_METHOD (input_method));

    meego_im_proxy_activate_context(input_method->priv->maliit_proxy);
    meego_im_proxy_show_input_method (input_method->priv->maliit_proxy);
}

/**
 * maliit_input_method_hide:
 * @input_method: (transfer none): The #MaliitInputMethod which you want to hide.
 *
 * Request to explicitly hide the Maliit virtual keyboard.
 */
void
maliit_input_method_hide (MaliitInputMethod *input_method)
{
    g_return_if_fail (MALIIT_IS_INPUT_METHOD (input_method));

    meego_im_proxy_activate_context(input_method->priv->maliit_proxy);
    meego_im_proxy_hide_input_method (input_method->priv->maliit_proxy);
}
