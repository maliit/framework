#include "dummyinputmethod.h"
#include <QDebug>

DummyInputMethod::DummyInputMethod(DuiInputContextConnection *connection)
    : DuiInputMethodBase(connection),
      setStateCount(0)
{
}

void DummyInputMethod::setState(const QList<DuiIMHandlerState> &state)
{
    qDebug() << __PRETTY_FUNCTION__ << state;
    ++setStateCount;
    setStateParam = state;
}

