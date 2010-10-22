#include "dummyimplugin3.h"
#include "dummyinputmethod3.h"

#include <QtPlugin>


DummyImPlugin3::DummyImPlugin3()
    : setStateCount(0)
{
    allowedStates << MInputMethod::OnScreen << MInputMethod::Hardware << MInputMethod::Accessory;
}

QString DummyImPlugin3::name() const
{
    return "DummyImPlugin3";
}

QStringList DummyImPlugin3::languages() const
{
    return QStringList("EN");
}

MInputMethodBase *
DummyImPlugin3::createInputMethod(MAbstractInputMethodHost *imHost)
{
    return new DummyInputMethod3(imHost);
}

MInputMethodSettingsBase *DummyImPlugin3::createInputMethodSettings()
{
    return 0;
}
QSet<MInputMethod::HandlerState> DummyImPlugin3::supportedStates() const
{
    return allowedStates;
}

Q_EXPORT_PLUGIN2(dummyimplugin3, DummyImPlugin3)

