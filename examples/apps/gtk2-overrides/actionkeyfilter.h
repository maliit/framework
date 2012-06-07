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

#ifndef THREE_FIELDS_ACTION_KEY_FILTER_H
#define THREE_FIELDS_ACTION_KEY_FILTER_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _ActionKeyFilter ActionKeyFilter;

ActionKeyFilter *
action_key_filter_new (GtkEntry *login,
                       GtkEntry *password);

void
action_key_filter_free (ActionKeyFilter *filter);

gboolean
action_key_filter_get_enter_login_accepts (ActionKeyFilter *filter);

void
action_key_filter_set_enter_login_accepts (ActionKeyFilter *filter,
                                           gboolean enter_login_accepts);

gboolean
action_key_filter_get_enter_password_accepts (ActionKeyFilter *filter);

void
action_key_filter_set_enter_password_accepts (ActionKeyFilter *filter,
                                              gboolean enter_password_accepts);

G_END_DECLS

#endif /* THREE_FIELDS_ACTION_KEY_FILTER_H */
