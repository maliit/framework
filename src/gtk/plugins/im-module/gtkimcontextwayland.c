/* GTK - The GIMP Toolkit
 * Copyright (C) 2013, 2017 Jan Arne Petersen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "gtkimcontextwayland.h"

#include "wayland-text-input-unstable-v1-client-protocol.h"

#include <gdk/gdk.h>
#include "gdk/gdkwayland.h"

#include <xkbcommon/xkbcommon.h>

#include <string.h>

struct _GtkIMContextWaylandPrivate
{
  struct zwp_text_input_v1 *text_input;

  GdkWindow *window;
  GdkRectangle cursor_rectangle;

  /* Current pre-edit */
  PangoAttrList *preedit_attrs;
  int32_t preedit_cursor;
  char *preedit_text;
  char *preedit_commit;

  /* Pending pre-edit */
  PangoAttrList *pending_preedit_attrs;
  int32_t pending_preedit_cursor;

  /* Pending commit */
  int32_t pending_delete_index;
  uint32_t pending_delete_length;
  int32_t pending_cursor;
  int32_t pending_anchor;

  uint32_t serial;
  uint32_t reset_serial;
};

static struct zwp_text_input_manager_v1 *text_input_manager = NULL;

G_DEFINE_DYNAMIC_TYPE (GtkIMContextWayland, gtk_im_context_wayland, GTK_TYPE_IM_CONTEXT);

static void
registry_handle_global (void               *data,
                        struct wl_registry *registry,
                        uint32_t            id,
                        const char         *interface,
                        uint32_t            version)
{
  if (strcmp (interface, "zwp_text_input_manager_v1") == 0)
    text_input_manager = wl_registry_bind (registry, id, &zwp_text_input_manager_v1_interface, 1);
}

static void
registry_handle_global_remove (void               *data,
                               struct wl_registry *registry,
                               uint32_t            name)
{
}

static const struct wl_registry_listener registry_listener =
{
  registry_handle_global,
  registry_handle_global_remove
};

static gpointer
bind_text_input_manager (gpointer data)
{
  GdkDisplay *display;
  struct wl_display *wl_display;
  struct wl_registry *registry;

  display = gdk_display_get_default ();
  wl_display = gdk_wayland_display_get_wl_display (display);
  registry = wl_display_get_registry (wl_display);
  wl_registry_add_listener (registry, &registry_listener, NULL);
  wl_display_dispatch (wl_display);

  return NULL;
}

static void
ensure_text_input_manager (void)
{
  static GOnce text_input_manager_once = G_ONCE_INIT;
  g_once (&text_input_manager_once, bind_text_input_manager, NULL);
}

static void
reset_preedit (GtkIMContextWayland *self)
{
  GtkIMContextWaylandPrivate *priv = self->priv;
  gboolean old_preedit;

  old_preedit = priv->preedit_text && priv->preedit_text[0] != '\0';

  g_clear_pointer (&priv->preedit_text, g_free);
  priv->preedit_cursor = 0;
  g_clear_pointer (&priv->preedit_attrs, (GDestroyNotify) pango_attr_list_unref);

  g_clear_pointer (&priv->pending_preedit_attrs, (GDestroyNotify) pango_attr_list_unref);
  priv->pending_preedit_cursor = 0;

  if (old_preedit)
    {
      g_signal_emit_by_name (self, "preedit-changed");
      g_signal_emit_by_name (self, "preedit-end");
    }
}

static gboolean
check_serial (GtkIMContextWayland *self,
              uint32_t             serial)
{
  GtkIMContextWaylandPrivate *priv = self->priv;

  return priv->serial - serial > priv->serial - priv->reset_serial;
}

static uint32_t
to_wayland_hints(GtkInputHints hints, GtkInputPurpose purpose) {
    uint32_t wl_hints = ZWP_TEXT_INPUT_V1_CONTENT_HINT_NONE;

    if (hints & GTK_INPUT_HINT_SPELLCHECK)
        wl_hints |= ZWP_TEXT_INPUT_V1_CONTENT_HINT_AUTO_CORRECTION;
    if (hints & GTK_INPUT_HINT_NO_SPELLCHECK)
        wl_hints &= ~ZWP_TEXT_INPUT_V1_CONTENT_HINT_AUTO_CORRECTION;
    if (hints & GTK_INPUT_HINT_WORD_COMPLETION)
        wl_hints |= ZWP_TEXT_INPUT_V1_CONTENT_HINT_AUTO_COMPLETION;
    if (hints & GTK_INPUT_HINT_LOWERCASE)
        wl_hints |= ZWP_TEXT_INPUT_V1_CONTENT_HINT_LOWERCASE;
    if (hints & GTK_INPUT_HINT_UPPERCASE_CHARS)
        wl_hints |= ZWP_TEXT_INPUT_V1_CONTENT_HINT_UPPERCASE;
    if (hints & GTK_INPUT_HINT_UPPERCASE_WORDS)
        wl_hints |= ZWP_TEXT_INPUT_V1_CONTENT_HINT_TITLECASE;
    if (hints & GTK_INPUT_HINT_UPPERCASE_SENTENCES)
        wl_hints |= ZWP_TEXT_INPUT_V1_CONTENT_HINT_AUTO_CAPITALIZATION;

    if (purpose == GTK_INPUT_PURPOSE_PASSWORD ||
        purpose == GTK_INPUT_PURPOSE_PIN) {
        wl_hints |= ZWP_TEXT_INPUT_V1_CONTENT_HINT_PASSWORD;
    }

    return wl_hints;
}

static uint32_t
to_wayland_purpose(GtkInputPurpose purpose) {
    switch (purpose) {
        case GTK_INPUT_PURPOSE_FREE_FORM:
            return ZWP_TEXT_INPUT_V1_CONTENT_PURPOSE_NORMAL;
        case GTK_INPUT_PURPOSE_ALPHA:
            return ZWP_TEXT_INPUT_V1_CONTENT_PURPOSE_ALPHA;
        case GTK_INPUT_PURPOSE_DIGITS:
            return ZWP_TEXT_INPUT_V1_CONTENT_PURPOSE_DIGITS;
        case GTK_INPUT_PURPOSE_NUMBER:
            return ZWP_TEXT_INPUT_V1_CONTENT_PURPOSE_NUMBER;
        case GTK_INPUT_PURPOSE_PHONE:
            return ZWP_TEXT_INPUT_V1_CONTENT_PURPOSE_PHONE;
        case GTK_INPUT_PURPOSE_URL:
            return ZWP_TEXT_INPUT_V1_CONTENT_PURPOSE_URL;
        case GTK_INPUT_PURPOSE_EMAIL:
            return ZWP_TEXT_INPUT_V1_CONTENT_PURPOSE_EMAIL;
        case GTK_INPUT_PURPOSE_NAME:
            return ZWP_TEXT_INPUT_V1_CONTENT_PURPOSE_NAME;
        case GTK_INPUT_PURPOSE_PASSWORD:
            return ZWP_TEXT_INPUT_V1_CONTENT_PURPOSE_PASSWORD;
        case GTK_INPUT_PURPOSE_PIN:
            return ZWP_TEXT_INPUT_V1_CONTENT_PURPOSE_DIGITS;
    }
}

static void
update_text_input_state (GtkIMContextWayland *self)
{
  GtkIMContextWaylandPrivate *priv = self->priv;
  char *surrounding = NULL;
  int cursor;
  GtkInputHints hints;
  GtkInputPurpose purpose;

  zwp_text_input_v1_set_cursor_rectangle (priv->text_input,
                                          priv->cursor_rectangle.x,
                                          priv->cursor_rectangle.y,
                                          priv->cursor_rectangle.width,
                                          priv->cursor_rectangle.height);

  if (gtk_im_context_get_surrounding (GTK_IM_CONTEXT (self),
                                      &surrounding,
                                      &cursor))
    {
      /* anchor is not supported in GtkIMContext */
      zwp_text_input_v1_set_surrounding_text (priv->text_input,
                                              surrounding,
                                              cursor, cursor);

      g_free (surrounding);
    }

  g_object_get (self,
                "input-hints", &hints,
                "input-purpose", &purpose,
                NULL);

  zwp_text_input_v1_set_content_type(priv->text_input,
                                     to_wayland_hints(hints, purpose),
                                     to_wayland_purpose(purpose));

  priv->serial += 1;
  zwp_text_input_v1_commit_state (priv->text_input, priv->serial);
}

static void
text_input_commit_string (void                     *data,
                          struct zwp_text_input_v1 *text_input,
                          uint32_t                  serial,
                          const char               *text)
{
  GtkIMContextWayland *self = GTK_IM_CONTEXT_WAYLAND (data);
  GtkIMContextWaylandPrivate *priv = self->priv;
  gchar *surrounding = NULL;
  gint cursor;
  glong delete_offset, delete_length;

  if (check_serial (self, serial))
    {
      /* Ignore commit ? reset_pending(); */
      return;
    }

  reset_preedit (self);

  if (priv->pending_delete_length)
    {
      gchar *p;

      gtk_im_context_get_surrounding (GTK_IM_CONTEXT (self),
                                      &surrounding,
                                      &cursor);

      p = surrounding + cursor;
      delete_offset = g_utf8_pointer_to_offset (p,
                                                p + priv->pending_delete_index);

      p += priv->pending_delete_index;
      delete_length = g_utf8_pointer_to_offset (p,
                                                p + priv->pending_delete_length);
      g_free (surrounding);

      gtk_im_context_delete_surrounding (GTK_IM_CONTEXT (self),
                                         delete_offset, delete_length);
    }

  g_signal_emit_by_name (self, "commit", text);

  priv->pending_delete_index = 0;
  priv->pending_delete_length = 0;
  priv->pending_cursor = 0;
  priv->pending_anchor = 0;

  /* GTK+ does not support to change cursor/anchor */

  update_text_input_state (self);
}

static void
text_input_preedit_string (void                     *data,
                           struct zwp_text_input_v1 *text_input,
                           uint32_t                  serial,
                           const char               *text,
                           const char               *commit)
{
  GtkIMContextWayland *self = GTK_IM_CONTEXT_WAYLAND (data);
  GtkIMContextWaylandPrivate *priv = self->priv;
  gboolean old_preedit;

  old_preedit = priv->preedit_text && priv->preedit_text[0] != '\0';

  if (check_serial (self, serial))
    return;

  priv->preedit_attrs = priv->pending_preedit_attrs;
  priv->pending_preedit_attrs = NULL;
  priv->preedit_cursor = priv->pending_preedit_cursor;
  priv->pending_preedit_cursor = 0;

  g_clear_pointer (&priv->preedit_text, g_free);
  priv->preedit_text = g_strdup (text);

  g_clear_pointer (&priv->preedit_commit, g_free);
  priv->preedit_commit = g_strdup (commit);

  if (!old_preedit)
    g_signal_emit_by_name (self, "preedit-start");

  g_signal_emit_by_name (self, "preedit-changed");

  if (priv->preedit_text && priv->preedit_text[0] != '\0')
    g_signal_emit_by_name (self, "preedit-end");

  update_text_input_state (self);
}

static void
text_input_delete_surrounding_text (void                     *data,
                                    struct zwp_text_input_v1 *text_input,
                                    int32_t                   index,
                                    uint32_t                  length)
{
  GtkIMContextWayland *self = GTK_IM_CONTEXT_WAYLAND (data);
  GtkIMContextWaylandPrivate *priv = self->priv;

  priv->pending_delete_index = index;
  priv->pending_delete_length = length;
}

static void
text_input_preedit_styling (void                     *data,
                            struct zwp_text_input_v1 *text_input,
                            uint32_t                  index,
                            uint32_t                  length,
                            uint32_t                  style)
{
  GtkIMContextWayland *self = GTK_IM_CONTEXT_WAYLAND (data);
  GtkIMContextWaylandPrivate *priv = self->priv;
  PangoAttribute *attr = NULL;

  if (length == 0)
    return;

  if (!priv->pending_preedit_attrs)
    priv->pending_preedit_attrs = pango_attr_list_new ();

  switch (style)
    {
    case ZWP_TEXT_INPUT_V1_PREEDIT_STYLE_DEFAULT:
    case ZWP_TEXT_INPUT_V1_PREEDIT_STYLE_UNDERLINE:
      attr = pango_attr_underline_new (PANGO_UNDERLINE_SINGLE);
      break;
    case ZWP_TEXT_INPUT_V1_PREEDIT_STYLE_INCORRECT:
      attr = pango_attr_underline_new (PANGO_UNDERLINE_ERROR);
      break;
    }

  if (attr)
    {
      attr->start_index = index;
      attr->end_index = index + length;
      pango_attr_list_insert (priv->pending_preedit_attrs, attr);
    }
}

static void
text_input_cursor_position (void                     *data,
                            struct zwp_text_input_v1 *text_input,
                            int32_t                   index,
                            int32_t                   anchor)
{
  GtkIMContextWayland *self = GTK_IM_CONTEXT_WAYLAND (data);
  GtkIMContextWaylandPrivate *priv = self->priv;

  priv->pending_cursor = index;
  priv->pending_anchor = anchor;
}

static void
text_input_preedit_cursor (void                     *data,
                           struct zwp_text_input_v1 *text_input,
                           int32_t                   index)
{
  GtkIMContextWayland *self = GTK_IM_CONTEXT_WAYLAND (data);

  self->priv->pending_preedit_cursor = index;
}

static void
text_input_modifiers_map (void                     *data,
                          struct zwp_text_input_v1 *text_input,
                          struct wl_array          *map)
{
}

static void
text_input_keysym (void                     *data,
                   struct zwp_text_input_v1 *text_input,
                   uint32_t                  serial,
                   uint32_t                  time,
                   uint32_t                  sym,
                   uint32_t                  state,
                   uint32_t                  modifiers)
{
  GtkIMContextWayland *self = GTK_IM_CONTEXT_WAYLAND (data);
  GtkIMContextWaylandPrivate *priv = self->priv;
  GdkDisplay *display;
  GdkSeat *seat;
  GdkEvent *event;
  struct timespec ts;
  GdkKeymapKey* keys;
  gint n_keys;

  display = gdk_display_get_default();
  seat = gdk_display_get_default_seat (display);

  reset_preedit(self);

  if (sym == XKB_KEY_NoSymbol)
    return;

  event = gdk_event_new (state ? GDK_KEY_PRESS : GDK_KEY_RELEASE);
  event->key.window = priv->window ? g_object_ref (priv->window) : NULL;
  event->key.send_event = FALSE;

  event->key.time = time;
  event->key.state = 0; // TODO add support for modifiers
  event->key.keyval = sym;

  if (event->key.keyval != 0 &&
      gdk_keymap_get_entries_for_keyval(gdk_keymap_get_default(),
                                        event->key.keyval, &keys, &n_keys)) {
    event->key.hardware_keycode = keys[0].keycode;
    event->key.group = keys[0].group;
    g_free(keys);
  }

  event->key.is_modifier = FALSE;
  gdk_event_set_device(event, gdk_seat_get_keyboard(seat));
  gdk_event_set_source_device(event, gdk_seat_get_keyboard(seat));

  gdk_event_put(event);
  gdk_event_free(event);
}

static void
text_input_enter (void                     *data,
                  struct zwp_text_input_v1 *text_input,
                  struct wl_surface        *surface)
{
  GtkIMContextWayland *self = GTK_IM_CONTEXT_WAYLAND (data);
  GtkIMContextWaylandPrivate *priv = self->priv;

  update_text_input_state (self);

  priv->reset_serial = priv->serial;
}

static void
text_input_leave (void                     *data,
                  struct zwp_text_input_v1 *text_input)
{
}

static void
text_input_input_panel_state (void                     *data,
                              struct zwp_text_input_v1 *text_input,
                              uint32_t                  state)
{
}

static void
text_input_language (void                     *data,
                     struct zwp_text_input_v1 *text_input,
                     uint32_t                  serial,
                     const char               *language)
{
  /* Not supported in GtkIMContext */
}

static void
text_input_text_direction (void                     *data,
                           struct zwp_text_input_v1 *text_input,
                           uint32_t                  serial,
                           uint32_t                  direction)
{
  /* Not supported in GtkIMContext */
}

static const struct zwp_text_input_v1_listener text_input_listener =
{
  text_input_enter,
  text_input_leave,
  text_input_modifiers_map,
  text_input_input_panel_state,
  text_input_preedit_string,
  text_input_preedit_styling,
  text_input_preedit_cursor,
  text_input_commit_string,
  text_input_cursor_position,
  text_input_delete_surrounding_text,
  text_input_keysym,
  text_input_language,
  text_input_text_direction
};

GtkIMContext *
gtk_im_context_wayland_new (void)
{
  return g_object_new (GTK_TYPE_IM_CONTEXT_WAYLAND, NULL);
}

static void
gtk_im_context_wayland_set_client_window (GtkIMContext *context,
                                          GdkWindow    *window)
{
  GtkIMContextWayland *self = GTK_IM_CONTEXT_WAYLAND (context);

  self->priv->window = window;
}

static void
gtk_im_context_wayland_get_preedit_string (GtkIMContext   *context,
                                           gchar         **str,
                                           PangoAttrList **attrs,
                                           gint           *cursor_pos)
{
  GtkIMContextWayland *self = GTK_IM_CONTEXT_WAYLAND (context);
  GtkIMContextWaylandPrivate *priv = self->priv;

  if (str != NULL)
    *str = g_strdup (priv->preedit_text ? priv->preedit_text : "");

  if (attrs != NULL)
    *attrs = priv->preedit_attrs ? pango_attr_list_ref (priv->preedit_attrs) : pango_attr_list_new ();

  if (cursor_pos != NULL)
    *cursor_pos = priv->preedit_cursor;
}

static void
gtk_im_context_wayland_focus_in (GtkIMContext *context)
{
  GtkIMContextWayland *self = GTK_IM_CONTEXT_WAYLAND (context);
  GtkIMContextWaylandPrivate *priv = self->priv;
  GdkDisplay *display;
  GdkSeat *seat;
  struct wl_surface *surface;

  g_return_if_fail (GDK_IS_WAYLAND_WINDOW (priv->window));
  g_return_if_fail (priv->text_input);

  surface = gdk_wayland_window_get_wl_surface (priv->window);

  if (!surface)
      return;

  display = gdk_display_get_default ();
  seat = gdk_display_get_default_seat (display);

  zwp_text_input_v1_show_input_panel (priv->text_input);
  zwp_text_input_v1_activate (priv->text_input,
                              gdk_wayland_seat_get_wl_seat (seat),
                              surface);
}

static void
commit_and_reset_preedit (GtkIMContextWayland *self)
{
  GtkIMContextWaylandPrivate *priv = self->priv;

  reset_preedit (self);

  if (priv->preedit_commit && priv->preedit_commit[0] != '\0')
    g_signal_emit_by_name (self, "commit", priv->preedit_commit);

  g_free (priv->preedit_commit);
  priv->preedit_commit = NULL;
}

static void
gtk_im_context_wayland_focus_out (GtkIMContext *context)
{
  GtkIMContextWayland *self = GTK_IM_CONTEXT_WAYLAND (context);
  GtkIMContextWaylandPrivate *priv = self->priv;
  GdkDisplay *display;
  GdkSeat *seat;

  g_return_if_fail (GDK_IS_WAYLAND_WINDOW (priv->window));
  g_return_if_fail (self->priv->text_input);

  display = gdk_display_get_default ();
  seat = gdk_display_get_default_seat (display);

  commit_and_reset_preedit (self);

  zwp_text_input_v1_deactivate (priv->text_input,
                                gdk_wayland_seat_get_wl_seat (seat));
}

static void
gtk_im_context_wayland_reset (GtkIMContext *context)
{
  GtkIMContextWayland *self = GTK_IM_CONTEXT_WAYLAND (context);
  GtkIMContextWaylandPrivate *priv = self->priv;

  g_return_if_fail (self->priv->text_input);

  commit_and_reset_preedit (self);

  zwp_text_input_v1_reset (priv->text_input);

  update_text_input_state (self);

  priv->reset_serial = priv->serial;
}

static void
gtk_im_context_wayland_set_cursor_location (GtkIMContext *context,
                                            GdkRectangle *area)
{
  GtkIMContextWayland *self = GTK_IM_CONTEXT_WAYLAND (context);
  GtkIMContextWaylandPrivate *priv = self->priv;

  g_return_if_fail (self->priv->text_input);

  priv->cursor_rectangle.x = area->x;
  priv->cursor_rectangle.y = area->y;
  priv->cursor_rectangle.width = area->width;
  priv->cursor_rectangle.height = area->height;

  update_text_input_state (self);
}

static void
gtk_im_context_wayland_set_use_preedit (GtkIMContext *context,
                                        gboolean      use_preedit)
{
}

static gboolean
gtk_im_context_wayland_filter_keypress (GtkIMContext *context,
                                        GdkEventKey  *event)
{
  GtkIMContextWayland *self;
  GtkIMContextWaylandPrivate *priv;
  GdkDisplay *display;
  GdkModifierType no_text_input_mask;

  g_return_val_if_fail (GTK_IS_IM_CONTEXT_WAYLAND(context), FALSE);
  g_return_val_if_fail (event, FALSE);

  self = GTK_IM_CONTEXT_WAYLAND (context);
  priv = self->priv;

  display = gdk_window_get_display (event->window);

  no_text_input_mask = gdk_keymap_get_modifier_mask (gdk_keymap_get_for_display (display),
                                                     GDK_MODIFIER_INTENT_NO_TEXT_INPUT);

  if (event->type == GDK_KEY_PRESS &&
      (event->state & no_text_input_mask) == 0)
  {
    gunichar ch;

    ch = gdk_keyval_to_unicode (event->keyval);
    if (ch != 0 && !g_unichar_iscntrl (ch)) {
      char utf8[10];
      int len;

      len = g_unichar_to_utf8(ch, utf8);
      utf8[len] = '\0';

      commit_and_reset_preedit(self);
      g_signal_emit_by_name(self, "commit", utf8);
      return TRUE;
    }
  }

  return FALSE;
}

static void
gtk_im_context_wayland_init (GtkIMContextWayland *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            GTK_TYPE_IM_CONTEXT_WAYLAND,
                                            GtkIMContextWaylandPrivate);

  ensure_text_input_manager ();
  if (text_input_manager)
    self->priv->text_input = zwp_text_input_manager_v1_create_text_input (text_input_manager);

  zwp_text_input_v1_add_listener (self->priv->text_input, &text_input_listener, self);
}

static void
gtk_im_context_wayland_finalize (GObject *obj)
{
  GtkIMContextWayland *self = GTK_IM_CONTEXT_WAYLAND (obj);

  pango_attr_list_unref (self->priv->preedit_attrs);
  pango_attr_list_unref (self->priv->pending_preedit_attrs);
  g_free (self->priv->preedit_text);
  g_free (self->priv->preedit_commit);

  G_OBJECT_CLASS (gtk_im_context_wayland_parent_class)->finalize (obj);
}

static void
gtk_im_context_wayland_class_init (GtkIMContextWaylandClass *klass)
{
  GtkIMContextClass *im_context_class = GTK_IM_CONTEXT_CLASS (klass);
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  im_context_class->set_client_window = gtk_im_context_wayland_set_client_window;
  im_context_class->get_preedit_string = gtk_im_context_wayland_get_preedit_string;
  im_context_class->focus_in = gtk_im_context_wayland_focus_in;
  im_context_class->focus_out = gtk_im_context_wayland_focus_out;
  im_context_class->reset = gtk_im_context_wayland_reset;
  im_context_class->set_cursor_location = gtk_im_context_wayland_set_cursor_location;
  im_context_class->set_use_preedit = gtk_im_context_wayland_set_use_preedit;
  im_context_class->filter_keypress = gtk_im_context_wayland_filter_keypress;

  object_class->finalize = gtk_im_context_wayland_finalize;

  g_type_class_add_private (klass, sizeof (GtkIMContextWaylandPrivate));
}

static void
gtk_im_context_wayland_class_finalize (GtkIMContextWaylandClass *klass)
{
}

void
gtk_im_context_wayland_register (GTypeModule *type_module)
{
  gtk_im_context_wayland_register_type (type_module);
}
