#include "dummyinputmethod3.h"
#include <QDebug>

DummyInputMethod3::DummyInputMethod3(DuiInputContextConnection *connection)
    : DuiInputMethodBase(connection),
      setStateCount(0)
{
}

void DummyInputMethod3::setState(const QList<DuiIMHandlerState> &state)
{
    qDebug() << __PRETTY_FUNCTION__ << state;
    ++setStateCount;
    setStateParam = state;
}

