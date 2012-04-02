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

MAbstractInputMethod *
DummyImPlugin2::createInputMethod(MAbstractInputMethodHost *,
                                  QWidget *)
{
    return 0;
}

MAbstractInputMethodSettings *DummyImPlugin2::createInputMethodSettings()
{
    return 0;
}

QSet<Maliit::HandlerState> DummyImPlugin2::supportedStates() const
{
    QSet<Maliit::HandlerState> result;

    result << Maliit::OnScreen << Maliit::Hardware << Maliit::Accessory;
    return result;
}

Q_EXPORT_PLUGIN2(dummyimplugin2, DummyImPlugin2)
