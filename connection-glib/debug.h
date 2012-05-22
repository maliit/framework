#ifndef _DEBUG_H
#define _DEBUG_H

#include <glib.h>

G_BEGIN_DECLS

gboolean maliit_is_debug_enabled(void);

#define DBG(x, a...) if (maliit_is_debug_enabled()) { g_debug("%s: " x, __FUNCTION__, ##a); }

#define STEP() DBG("")

#define UNUSED(v) (void)v;

G_END_DECLS

#endif // _DEBUG_H
