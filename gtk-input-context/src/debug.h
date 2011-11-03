#ifndef _DEBUG_H
#define _DEBUG_H

#include <glib.h>
#include <glib/gprintf.h>

G_BEGIN_DECLS

#if defined(ENABLE_DEBUG)
#define DBG(x, a...) g_fprintf (stderr,  __FILE__ ",%d,%s: " x "\n", __LINE__, __func__, ##a)
#else
#define DBG(x, a...) do {} while (0)
#endif

#define STEP() DBG("")

#define UNUSED(v) (void)v;

G_END_DECLS
#endif // _DEBUG_H
