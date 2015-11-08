/* This file is part of Maliit framework
 *
 * Copyright (C) 2012 Canonical Ltd
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
#include <maliit-glib/maliitsettingsmanager.h>
#include <maliit-glib/maliitsettingsentry.h>

#include "mockmaliitserver.h"

/**
 * ut_maliit_glib_settings: Unit tests for the settings API of maliit-glib
 *
 * Testing is done using the public API of maliit-glib.
 * maliit-server is mocked out due to the difficulties of hosting it in
 * the tests and connecting the client to it (pulls in DBus session).
 *
 *  ---------------                 ------------------
 *  | maliit-glib |                 |     MOCK       |
 *  |  settings   |  Connection API |  maliit-server |
 *  |  component  |   <---------->  |                |
 *  ---------------                 ------------------
 */

/*
 * TODO:
 *
 * - Test that maliit_settings_entry_is_value_valid returns FALSE for incorrect values,
 *   and TRUE for correct values; for all of the different setting types.
 *   Simply need to enumerate a set of data and expected result
 *   Each data pair should be registered as a separate test
 *   This can be done with g_test_add_data_func
 *
 * - Test that the plugin settings are received correctly,
 *   and that the MaliitSettingsEntry APIs give access to the data.
 *   Can be done the same way as test_load_plugins_settings_returns_settings,
 *   but populating the MockMaliitServer::settings with DBus data.
 *   Verify result through:
 *   maliit_settings_entry_get_description
 *   maliit_settings_entry_get_key
 *   maliit_settings_entry_get_entry_type
 *   maliit_settings_entry_get_value
 *   maliit_settings_entry_get_attributes
 *   maliit_settings_entry_is_current_value_valid
 *
 * - Test that maliit_settings_manager_set_preferred_description_locale()
 *   results in the server replying with descriptions in that locale.
 *   Can be done by passing fake Bus settings data, including descriptions
 *   and implementing in MockMaliitServer a translation mechanism that respects
 *   the preferred locale.
 *
 * - Test that setting a value using maliit_settings_entry_set_value
 *   results in the server being called with the updated value.
 *   Can be done by observing meego_im_proxy_set_extended_attribute
 *   (wrap this implementation detail in MockMaliitServer)
 *
 * - Test that when a setting entry is updated on server side,
 *   the client is notified through MaliitSettingsEntry::value-changed.
 *   Can be done by emulating the meego_imcontext_notify_extended_attribute_changed
 *   from maliit-server (add to MockMaliitServer)
 */

/**
 * Test that #MaliitSettingsManager has a default for preferred_description_locale
 */
void
test_preferred_description_locale_default(void)
{
    const gchar *preferred_description_locale = maliit_settings_manager_get_preferred_description_locale();
    g_assert_cmpstr(preferred_description_locale, ==, "en");
}

/**
 * Test that #MaliitSettingsManager has a default for preferred_description_locale
 */
void
test_preferred_description_locale_set_get_roundtrip(void)
{
    const char *expected = "no_NB";
    maliit_settings_manager_set_preferred_description_locale(expected);
    const gchar *actual = maliit_settings_manager_get_preferred_description_locale();
    g_assert_cmpstr(actual, ==, expected);
}

typedef struct {
    gboolean received;
    GList *settings;
} OnPluginSettingsreceivedState;

void
add_gobject_ref_gfunc(gpointer data, gpointer user_data G_GNUC_UNUSED)
{
    g_object_ref(data);
}

void
on_plugin_settings_received(MaliitSettingsManager *manager G_GNUC_UNUSED,
                            GList *settings, gpointer user_data)
{
    OnPluginSettingsreceivedState *state = (OnPluginSettingsreceivedState *)user_data;
    state->received = TRUE;
    state->settings = g_list_copy(settings);
    g_list_foreach(settings, add_gobject_ref_gfunc, NULL);
}

/**
 * Test that calling maliit_settings_manager_load_plugins will return
 * a list of settings through the MaliitSettingsManager::plugin-settings-received signal
 */
void
test_load_plugins_settings_returns_settings(void)
{
    MockMaliitServer *server = mock_maliit_server_new();
    MaliitSettingsManager *manager = maliit_settings_manager_new();
    OnPluginSettingsreceivedState state = {FALSE, NULL};
    server->settings = g_ptr_array_new();

    g_signal_connect(manager, "plugin-settings-received",
                     G_CALLBACK(on_plugin_settings_received), &state);
    maliit_settings_manager_load_plugin_settings(manager);
    g_assert(server->load_plugin_settings_called);
    g_assert(state.received);
    g_assert_cmpint(g_list_length(state.settings), ==, 0);

    g_list_free_full(state.settings, g_object_unref);
    g_object_unref(manager);
    mock_maliit_server_free(server);
}


void
on_signal_received(gpointer instance G_GNUC_UNUSED, gpointer user_data)
{
    gboolean *received = (gboolean *)user_data;
    *received = TRUE;
}

int
main (int argc, char **argv) {
    g_test_init(&argc, &argv, NULL);
#if !(GLIB_CHECK_VERSION(2, 35, 0))
    g_type_init();
#endif

    g_test_add_func("/ut_maliit_glib_settings/MaliitSettingsManager/preferred_description_locale/default",
                    test_preferred_description_locale_default);
    g_test_add_func("/ut_maliit_glib_settings/MaliitSettingsManager/preferred_description_locale/set-get-roundtrip",
                    test_preferred_description_locale_set_get_roundtrip);
    g_test_add_func("/ut_maliit_glib_settings/MaliitSettingsManager/load_plugin_settings/returns-settings",
                    test_load_plugins_settings_returns_settings);

    return g_test_run();
}
