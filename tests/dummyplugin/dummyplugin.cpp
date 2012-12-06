#include "dummyplugin.h"

#include <QtPlugin>


QString DummyPlugin::name()
{
    return "DummyPlugin";
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(dummyplugin, DummyPlugin)
#endif
