#ifndef _QT_KEYSYM_MAP_H
#define _QT_KEYSYM_MAP_H

#include <glib.h>

G_BEGIN_DECLS

int QtKeyToXKeySym(int qtKey);
int XKeySymToQTKey(uint keySym);


G_END_DECLS

#endif //_QT_KEYSYM_MAP_H
