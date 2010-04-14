#include "dummyimplugin3.h"
#include "dummyinputmethod3.h"

#include <QtPlugin>


DummyImPlugin3::DummyImPlugin3()
    : setStateCount(0)
{
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
DummyImPlugin3::createInputMethod(MInputContextConnection *icConnection)
{
    return new DummyInputMethod3(icConnection);
}

Q_EXPORT_PLUGIN2(dummyimplugin3, DummyImPlugin3)

