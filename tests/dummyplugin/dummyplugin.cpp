#include "dummyplugin.h"

#include <QtPlugin>


QString DummyPlugin::name()
{
    return "DummyPlugin";
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(dummyplugin, DummyPlugin)
#endif
