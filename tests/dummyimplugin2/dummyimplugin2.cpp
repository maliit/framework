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

QSet<Maliit::HandlerState> DummyImPlugin2::supportedStates() const
{
    QSet<Maliit::HandlerState> result;

    result << Maliit::OnScreen << Maliit::Hardware << Maliit::Accessory;
    return result;
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(dummyimplugin2, DummyImPlugin2)
#endif
