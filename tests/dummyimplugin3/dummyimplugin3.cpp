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
DummyImPlugin3::createInputMethod(MAbstractInputMethodHost *host,
                                  QWidget *mainWindow)
{
    return new DummyInputMethod3(host, mainWindow);
}

QSet<Maliit::HandlerState> DummyImPlugin3::supportedStates() const
{
    return allowedStates;
}

Q_EXPORT_PLUGIN2(dummyimplugin3, DummyImPlugin3)

