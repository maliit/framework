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

QStringList DummyImPlugin3::languages() const
{
    return QStringList("EN");
}

MAbstractInputMethod *
DummyImPlugin3::createInputMethod(MAbstractInputMethodHost *host,
                                  QWidget *mainWindow)
{
    return new DummyInputMethod3(host, mainWindow);
}

MAbstractInputMethodSettings *DummyImPlugin3::createInputMethodSettings()
{
    return 0;
}
QSet<Maliit::HandlerState> DummyImPlugin3::supportedStates() const
{
    return allowedStates;
}

Q_EXPORT_PLUGIN2(dummyimplugin3, DummyImPlugin3)

