#include "dummyimplugin.h"
#include "dummyinputmethod.h"

#include <QtPlugin>


DummyImPlugin::DummyImPlugin()
{
    allowedStates << OnScreen << Hardware << Accessory;
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

MInputMethodSettingsBase *DummyImPlugin::createInputMethodSettings()
{
    return 0;
}

QSet<MIMHandlerState> DummyImPlugin::supportedStates() const
{
    return allowedStates;
}

Q_EXPORT_PLUGIN2(dummyimplugin, DummyImPlugin)

