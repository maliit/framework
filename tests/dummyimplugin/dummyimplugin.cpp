#include "dummyimplugin.h"
#include "dummyinputmethod.h"

#include <QtPlugin>


DummyImPlugin::DummyImPlugin()
{
    allowedStates << Maliit::OnScreen << Maliit::Hardware << Maliit::Accessory;
}

QString DummyImPlugin::name() const
{
    return "DummyImPlugin";
}

MAbstractInputMethod *
DummyImPlugin::createInputMethod(MAbstractInputMethodHost *host)
{
    return new DummyInputMethod(host);
}

QSet<Maliit::HandlerState> DummyImPlugin::supportedStates() const
{
    return allowedStates;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(dummyimplugin, DummyImPlugin)
#endif
