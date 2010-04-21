#include "dummyinputmethod3.h"
#include <QDebug>

DummyInputMethod3::DummyInputMethod3(MInputContextConnection *connection)
    : MInputMethodBase(connection),
      setStateCount(0),
      switchContextCallCount(0),
      directionParam(M::SwitchUndefined),
      enableAnimationParam(false)
{
}

void DummyInputMethod3::setState(const QSet<MIMHandlerState> &state)
{
    qDebug() << __PRETTY_FUNCTION__ << state;
    ++setStateCount;
    setStateParam = state;
}

void DummyInputMethod3::switchContext(M::InputMethodSwitchDirection direction,
                              bool enableAnimation)
{
    ++switchContextCallCount;
    directionParam = direction;
    enableAnimationParam = enableAnimation;
}

