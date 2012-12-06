#include "dummyimplugin3.h"
#include "dummyinputmethod3.h"

#include <QtPlugin>


DummyImPlugin3::DummyImPlugin3()
    : setStateCount(0)
{
    allowedStates << Maliit::OnScreen << Maliit::Hardware << Maliit::Accessory;
}

QString DummyImPlugin3::name() const
{
    return "DummyImPlugin3";
}

MAbstractInputMethod *
DummyImPlugin3::createInputMethod(MAbstractInputMethodHost *host)
{
    return new DummyInputMethod3(host);
}

QSet<Maliit::HandlerState> DummyImPlugin3::supportedStates() const
{
    return allowedStates;
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(dummyimplugin3, DummyImPlugin3)
#endif
