#include "dummyimplugin.h"
#include "dummyinputmethod.h"

#include <QtPlugin>


DummyImPlugin::DummyImPlugin()
{
    allowedStates << MInputMethod::OnScreen << MInputMethod::Hardware << MInputMethod::Accessory;
}

QString DummyImPlugin::name() const
{
    return "DummyImPlugin";
}

QStringList DummyImPlugin::languages() const
{
    return QStringList("EN");
}

MAbstractInputMethod *
DummyImPlugin::createInputMethod(MAbstractInputMethodHost *host,
                                 QWidget *mainWindow)
{
    return new DummyInputMethod(host, mainWindow);
}

QSet<MInputMethod::HandlerState> DummyImPlugin::supportedStates() const
{
    return allowedStates;
}

Q_EXPORT_PLUGIN2(dummyimplugin, DummyImPlugin)

