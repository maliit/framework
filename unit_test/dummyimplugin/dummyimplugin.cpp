#include "dummyimplugin.h"
#include "dummyinputmethod.h"

#include <QtPlugin>


DummyImPlugin::DummyImPlugin()
{
}

QString DummyImPlugin::name() const
{
    return "DummyImPlugin";
}

QStringList DummyImPlugin::languages() const
{
    return QStringList("EN");
}

MInputMethodBase *
DummyImPlugin::createInputMethod(MInputContextConnection *icConnection)
{
    return new DummyInputMethod(icConnection);
}

Q_EXPORT_PLUGIN2(dummyimplugin, DummyImPlugin)

