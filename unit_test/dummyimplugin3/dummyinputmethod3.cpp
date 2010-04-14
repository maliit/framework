#include "dummyinputmethod3.h"
#include <QDebug>

DummyInputMethod3::DummyInputMethod3(MInputContextConnection *connection)
    : MInputMethodBase(connection),
      setStateCount(0)
{
}

void DummyInputMethod3::setState(const QList<MIMHandlerState> &state)
{
    qDebug() << __PRETTY_FUNCTION__ << state;
    ++setStateCount;
    setStateParam = state;
}

