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

DuiInputMethodBase *
DummyImPlugin2::createInputMethod(DuiInputContextConnection * /* icConnection */)
{
    return NULL;
}

Q_EXPORT_PLUGIN2(dummyimplugin2, DummyImPlugin2)
