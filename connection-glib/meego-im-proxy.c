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

#include <string.h>

#include "meego-im-proxy.h"
#include "meego-im-proxy-glue.h"
#include "debug.h"

#include <maliit-glib/maliitattributeextensionregistry.h>

#include <dbus/dbus-glib.h>
G_DEFINE_TYPE(MeegoIMProxy, meego_im_proxy, G_TYPE_OBJECT);

#define MEEGO_IM_OBJECT_PATH "/com/meego/inputmethod/uiserver1"
#define MEEGO_IM_SERVICE_INTERFACE "com.meego.inputmethod.uiserver1"

static void meego_im_proxy_finalize(GObject *object);
static void meego_im_proxy_class_init(MeegoIMProxyClass *klass);
static void meego_im_proxy_init(MeegoIMProxy *meego_im_proxy);

enum {
    SIGNAL_CONNECTION_DROPPED = 0,
    N_SIGNALS
};

static guint meego_im_proxy_signals[N_SIGNALS];

/* Private struct */
struct _MeegoImProxyPrivate {
    DBusGProxy *dbusproxy;
    MaliitAttributeExtensionRegistry *registry;
    guint extension_registered_id;
    guint extension_unregistered_id;
    guint extension_changed_id;
};

static void
handle_disconnect(gpointer instance, MeegoIMProxy *im_proxy)
{
    g_return_if_fail(im_proxy);

    im_proxy->priv->dbusproxy = NULL;
    g_signal_emit(im_proxy, meego_im_proxy_signals[SIGNAL_CONNECTION_DROPPED], 0, NULL);

}

MeegoIMProxy *
meego_im_proxy_get_singleton(void)
{
    static MeegoIMProxy *proxy = NULL;
    if (!proxy)
        proxy = g_object_new(MEEGO_TYPE_IM_PROXY, NULL);
    return proxy;
}


static void
meego_im_proxy_finalize(GObject *object)
{
    MeegoIMProxy *proxy = MEEGO_IM_PROXY (object);
    MeegoImProxyPrivate *priv = proxy->priv;

    if (priv->extension_registered_id) {
        g_signal_handler_disconnect (priv->registry, priv->extension_registered_id);
        priv->extension_registered_id = 0;
    }
    if (priv->extension_unregistered_id) {
        g_signal_handler_disconnect (priv->registry, priv->extension_unregistered_id);
        priv->extension_unregistered_id = 0;
    }
    if (priv->extension_changed_id) {
        g_signal_handler_disconnect (priv->registry, priv->extension_changed_id);
        priv->extension_changed_id = 0;
    }

    g_clear_object (&priv->registry);

    G_OBJECT_CLASS(meego_im_proxy_parent_class)->finalize(object);
}


static void
meego_im_proxy_class_init(MeegoIMProxyClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->finalize = meego_im_proxy_finalize;

    meego_im_proxy_signals[SIGNAL_CONNECTION_DROPPED] =
        g_signal_new("connection-dropped", G_TYPE_FROM_CLASS(klass),
                     0, 0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

    g_type_class_add_private(klass, sizeof(MeegoImProxyPrivate));
}

static void
meego_im_proxy_extension_registered (MeegoIMProxy *proxy,
                                     gint id,
                                     const gchar *filename,
                                     gpointer user_data G_GNUC_UNUSED)
{
    meego_im_proxy_register_extension (proxy, id, filename);
}

static void
meego_im_proxy_extension_unregistered (MeegoIMProxy *proxy,
                                       gint id,
                                       gpointer user_data G_GNUC_UNUSED)
{
    meego_im_proxy_unregister_extension (proxy, id);
}

#ifndef G_VALUE_INIT
#define G_VALUE_INIT  { 0, { { 0 } } }
#endif

static void
meego_im_proxy_extension_changed (MeegoIMProxy *proxy,
                                  gint id,
                                  const gchar *key,
                                  GVariant *value,
                                  gpointer user_data G_GNUC_UNUSED)
{
    /* key + 1 so we won't split the first slash */
    gchar **parts = g_strsplit (key + 1, "/", -1);

    STEP ();

    if (parts) {
        if (g_strv_length (parts) == 3) {
            GValue g_value = G_VALUE_INIT;

            dbus_g_value_parse_g_variant (value, &g_value);
            if (G_VALUE_TYPE (&g_value) != G_TYPE_INVALID) {
                gchar *target = g_strdup_printf ("/%s", parts[0]);

                meego_im_proxy_set_extended_attribute (proxy,
                                                       id,
                                                       target,
                                                       parts[1],
                                                       parts[2],
                                                       &g_value);
                g_free (target);
                g_value_unset (&g_value);
            } else {
              g_warning ("Could not convert variant into value");
            }
        }
        g_strfreev (parts);
    }
}

static void
meego_im_proxy_init(MeegoIMProxy *self)
{
    MeegoImProxyPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (self, MEEGO_TYPE_IM_PROXY, MeegoImProxyPrivate);

    priv->dbusproxy = NULL;
    priv->registry = maliit_attribute_extension_registry_get_instance ();
    priv->extension_registered_id = g_signal_connect_swapped (priv->registry,
                                                              "extension-registered",
                                                              G_CALLBACK (meego_im_proxy_extension_registered),
                                                              self);
    priv->extension_unregistered_id = g_signal_connect_swapped (priv->registry,
                                                                "extension-unregistered",
                                                                G_CALLBACK (meego_im_proxy_extension_unregistered),
                                                                self);
    priv->extension_changed_id = g_signal_connect_swapped (priv->registry,
                                                           "extension-changed",
                                                           G_CALLBACK (meego_im_proxy_extension_changed),
                                                           self);
    self->priv = priv;
}

static void
register_all_extensions (MeegoIMProxy *proxy)
{
    GList *extensions = maliit_attribute_extension_registry_get_extensions (proxy->priv->registry);
    GList *iter;

    STEP();

    for (iter = extensions; iter; iter = iter->next) {
        MaliitAttributeExtension *extension = MALIIT_ATTRIBUTE_EXTENSION (iter->data);

        DBG("registering maliit extension attribute - id: %d", maliit_attribute_extension_get_id (extension));
        if (!meego_im_proxy_register_extension (proxy,
                                                maliit_attribute_extension_get_id (extension),
                                                maliit_attribute_extension_get_filename (extension))) {
            g_warning ("could not register an extension in mass registerer - no proxy");
        } else {
            GHashTable *attributes = maliit_attribute_extension_get_attributes (extension);
            GHashTableIter attributes_iter;
            gpointer key;
            gpointer value;
            gint id = maliit_attribute_extension_get_id (extension);

            g_hash_table_iter_init (&attributes_iter, attributes);

            while (g_hash_table_iter_next (&attributes_iter, &key, &value)) {
                meego_im_proxy_extension_changed (proxy, id, key, value, NULL);
            }
        }
    }

    g_list_free (extensions);
}

void
meego_im_proxy_connect(MeegoIMProxy *proxy, gpointer connection)
{
    DBusGProxy *dbusproxy;
    DBusGConnection *dbus_connection = (DBusGConnection *)connection;
    g_return_if_fail(dbus_connection != NULL);

    if (proxy->priv->dbusproxy) {
        g_object_unref(proxy->priv->dbusproxy);
    }

    dbusproxy = dbus_g_proxy_new_for_peer(dbus_connection,
                                          MEEGO_IM_OBJECT_PATH, /* obj path */
                                          MEEGO_IM_SERVICE_INTERFACE /* interface */);

    if (dbusproxy == NULL) {
        g_warning("could not create dbus_proxy\n");
    }

    g_signal_connect(G_OBJECT(dbusproxy), "destroy", G_CALLBACK(handle_disconnect), proxy);

    proxy->priv->dbusproxy = dbusproxy;

    register_all_extensions (proxy);
}

gboolean
meego_im_proxy_activate_context(MeegoIMProxy *proxy)
{
    GError *error = NULL;
    gboolean ret = TRUE;

    STEP();
    if (!proxy || proxy->priv->dbusproxy == NULL)
        return FALSE;

    ret = com_meego_inputmethod_uiserver1_activate_context(proxy->priv->dbusproxy, &error);

    if (error != NULL) {
        g_warning("%s", error->message);
    }

    return ret;
}

gboolean
meego_im_proxy_register_extension (MeegoIMProxy *proxy,
                                   gint id,
                                   const gchar *filename)
{
    GError *error = NULL;
    gboolean ret = TRUE;

    STEP();

    if (!proxy || proxy->priv->dbusproxy == NULL)
        return FALSE;

    ret = com_meego_inputmethod_uiserver1_register_attribute_extension (proxy->priv->dbusproxy,
                                                                        id,
                                                                        filename,
                                                                        &error);

    if (error != NULL) {
      g_warning("%s", error->message);
    }

    return ret;
}

gboolean
meego_im_proxy_unregister_extension (MeegoIMProxy *proxy,
                                     gint id)
{
    GError *error = NULL;
    gboolean ret = TRUE;

    STEP();

    if (!proxy || proxy->priv->dbusproxy == NULL)
        return FALSE;

    ret = com_meego_inputmethod_uiserver1_unregister_attribute_extension (proxy->priv->dbusproxy,
                                                                          id,
                                                                          &error);

    if (error != NULL) {
      g_warning("%s", error->message);
    }

    return ret;
}

gboolean
meego_im_proxy_app_orientation_changed(MeegoIMProxy *proxy, const gint angle)
{
    GError *error = NULL;
    gboolean ret = TRUE;

    STEP();
    if (!proxy || proxy->priv->dbusproxy == NULL)
        return FALSE;

    ret = com_meego_inputmethod_uiserver1_app_orientation_changed(proxy->priv->dbusproxy, angle, &error);

    if (error != NULL) {
        g_warning("%s", error->message);
    }

    return ret;
}


gboolean
meego_im_proxy_hide_input_method(MeegoIMProxy *proxy)
{
    GError *error = NULL;
    gboolean ret = TRUE;

    STEP();
    if (!proxy || proxy->priv->dbusproxy == NULL)
        return FALSE;

    ret = com_meego_inputmethod_uiserver1_hide_input_method(proxy->priv->dbusproxy, &error);

    if (error != NULL) {
        g_warning("%s", error->message);
    }

    return ret;
}


#if 0
// Not yet really implemented

gboolean
meego_im_proxy_mouse_clicked_on_preedit(MeegoIMProxy *proxy, const GValueArray *pos,
                                        const GValueArray *preedit_rect)
{
    GError *error = NULL;
    gboolean ret = TRUE;

    if (!proxy || proxy->priv->dbusproxy == NULL)
        return FALSE;

    ret = com_meego_inputmethod_uiserver1_mouse_clicked_on_preedit(proxy->priv->dbusproxy,
            pos, preedit_rect, &error);

    if (error != NULL) {
        g_warning("%s", error->message);
    }

    return ret;
}
#endif

gboolean
meego_im_proxy_update_widget_info(MeegoIMProxy *proxy, const GHashTable *state_information, gboolean focus_changed)
{
    GError *error = NULL;
    gboolean ret = TRUE;

    STEP();
    if (!proxy || proxy->priv->dbusproxy == NULL)
        return FALSE;

    ret = com_meego_inputmethod_uiserver1_update_widget_information(proxy->priv->dbusproxy,
            state_information, focus_changed, &error);

    if (error != NULL) {
        g_warning("%s", error->message);
    }

    return ret;
}




gboolean
meego_im_proxy_process_key_event(MeegoIMProxy *proxy, const gint type, const gint code,
                                 const gint modifiers, const char *text,
                                 const gboolean auto_repeat, const gint count,
                                 const guint native_scan_code,
                                 const guint native_modifiers,
                                 const guint time)
{
    GError *error = NULL;
    gboolean ret = TRUE;

    DBG("QT key event type=0x%x, code=0x%x, modifiers=0x%x, text=%s",
        type, code, modifiers, text);
    if (!proxy || proxy->priv->dbusproxy == NULL)
        return FALSE;

    ret = com_meego_inputmethod_uiserver1_process_key_event(proxy->priv->dbusproxy,
            type, code, modifiers, text, auto_repeat,
            count, native_scan_code, native_modifiers,
            time, &error);

    if (error != NULL) {
        g_warning("%s", error->message);
    }

    return ret;
}


gboolean
meego_im_proxy_reset(MeegoIMProxy *proxy)
{
    GError *error = NULL;
    gboolean ret = TRUE;

    STEP();
    if (!proxy || proxy->priv->dbusproxy == NULL)
        return FALSE;

    ret = com_meego_inputmethod_uiserver1_reset(proxy->priv->dbusproxy, &error);

    if (error != NULL) {
        g_warning("%s", error->message);
    }

    return ret;
}


gboolean
meego_im_proxy_set_copy_paste_state(MeegoIMProxy *proxy, const gboolean copy_available,
                                    const gboolean paste_available)
{
    GError *error = NULL;
    gboolean ret = TRUE;

    STEP();
    if (!proxy || proxy->priv->dbusproxy == NULL)
        return FALSE;

    ret = com_meego_inputmethod_uiserver1_set_copy_paste_state(proxy->priv->dbusproxy,
            copy_available, paste_available, &error);

    if (error != NULL) {
        g_warning("%s", error->message);
    }

    return ret;
}


gboolean
meego_im_proxy_set_preedit(MeegoIMProxy *proxy, const char *text, gint cursor_pos)
{
    GError *error = NULL;
    gboolean ret = TRUE;

    STEP();
    if (!proxy || proxy->priv->dbusproxy == NULL)
        return FALSE;

    ret = com_meego_inputmethod_uiserver1_set_preedit(proxy->priv->dbusproxy, text, cursor_pos, &error);

    if (error != NULL) {
        g_warning("%s", error->message);
    }

    return ret;
}


gboolean
meego_im_proxy_show_input_method(MeegoIMProxy *proxy)
{
    GError *error = NULL;
    gboolean ret = TRUE;

    STEP();
    if (!proxy || proxy->priv->dbusproxy == NULL)
        return FALSE;

    ret = com_meego_inputmethod_uiserver1_show_input_method(proxy->priv->dbusproxy, &error);

    if (error != NULL) {
        g_warning("%s", error->message);
    }

    return ret;
}

gboolean
meego_im_proxy_set_extended_attribute (MeegoIMProxy *proxy,
                                       gint id,
                                       const gchar *target,
                                       const gchar *target_item,
                                       const gchar *attribute,
                                       GValue *value)
{
    GError *error = NULL;
    gboolean ret = TRUE;

    STEP();
    if (!proxy || proxy->priv->dbusproxy == NULL)
        return FALSE;

    ret = com_meego_inputmethod_uiserver1_set_extended_attribute (proxy->priv->dbusproxy,
                                                                  id,
                                                                  target,
                                                                  target_item,
                                                                  attribute,
                                                                  value,
                                                                  &error);

    if (error != NULL) {
        g_warning("%s", error->message);
    }

    return ret;
}
