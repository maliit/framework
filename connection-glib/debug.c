#include "debug.h"

#include <string.h>

gboolean
maliit_is_debug_enabled(void)
{
    static gint debug_enabled = -1;

    if (debug_enabled == -1) {
        const char *enabled_debug_var = g_getenv("MALIIT_DEBUG");
        if (enabled_debug_var && strlen(enabled_debug_var) > 0
            && strcmp(enabled_debug_var, "0") != 0) {
            debug_enabled = 1;
        } else {
            debug_enabled = 0;
        }
    }

    return debug_enabled == 1;
}
