#include "dummyimplugin2.h"

#include <QtPlugin>


QString DummyImPlugin2::name() const
{
    return "DummyImPlugin2";
}

MAbstractInputMethod *
DummyImPlugin2::createInputMethod(MAbstractInputMethodHost *)
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
