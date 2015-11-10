/* This file is part of Maliit framework
 *
 * Copyright (C) 2015 Canonical Ltd
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

#ifndef MALIIT_GLIB_BUS_H
#define MALIIT_GLIB_BUS_H

#include <gio/gio.h>

#include "maliitserver.h"
#include "maliitcontext.h"

G_BEGIN_DECLS

gboolean        maliit_is_running         (void);

void            maliit_get_server         (GCancellable         *cancellable,
                                           GAsyncReadyCallback   callback,
                                           gpointer              user_data);

MaliitServer *  maliit_get_server_finish  (GAsyncResult         *res,
                                           GError              **error);

MaliitServer *  maliit_get_server_sync    (GCancellable         *cancellable,
                                           GError              **error);

void            maliit_get_context        (GCancellable         *cancellable,
                                           GAsyncReadyCallback   callback,
                                           gpointer              user_data);

MaliitContext * maliit_get_context_finish (GAsyncResult         *res,
                                           GError              **error);

MaliitContext * maliit_get_context_sync   (GCancellable         *cancellable,
                                           GError              **error);

G_END_DECLS

#endif /* MALIIT_GLIB_BUS_H */
