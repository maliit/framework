#include "dummyplugin.h"

#include <QtPlugin>


QString DummyPlugin::name()
{
    return "DummyPlugin";
}

Q_EXPORT_PLUGIN2(dummyplugin, DummyPlugin)

