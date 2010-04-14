#include "dummyinputmethod.h"
#include <QDebug>

DummyInputMethod::DummyInputMethod(MInputContextConnection *connection)
    : MInputMethodBase(connection),
      setStateCount(0)
{
}

void DummyInputMethod::setState(const QList<MIMHandlerState> &state)
{
    qDebug() << __PRETTY_FUNCTION__ << state;
    ++setStateCount;
    setStateParam = state;
}

