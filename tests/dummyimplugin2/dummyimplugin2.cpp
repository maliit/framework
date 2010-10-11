#include "dummyimplugin2.h"

#include <QtPlugin>


QString DummyImPlugin2::name() const
{
    return "DummyImPlugin2";
}

QStringList DummyImPlugin2::languages() const
{
    return QStringList("EN");
}

MInputMethodBase *
DummyImPlugin2::createInputMethod(MInputContextConnection * /* icConnection */)
{
    return NULL;
}

MInputMethodSettingsBase *DummyImPlugin2::createInputMethodSettings()
{
    return 0;
}

QSet<MInputMethod::HandlerState> DummyImPlugin2::supportedStates() const
{
    QSet<MInputMethod::HandlerState> result;

    result << MInputMethod::OnScreen << MInputMethod::Hardware << MInputMethod::Accessory;
    return result;
}

Q_EXPORT_PLUGIN2(dummyimplugin2, DummyImPlugin2)
