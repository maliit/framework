#include "dummyinputmethod.h"
#include <QDebug>
#include <QTimer>

DummyInputMethod::DummyInputMethod(MInputContextConnection *connection)
    : MInputMethodBase(connection),
      setStateCount(0),
      switchContextCallCount(0),
      directionParam(M::SwitchUndefined),
      enableAnimationParam(false)
{
}

void DummyInputMethod::setState(const QSet<MIMHandlerState> &state)
{
    qDebug() << __PRETTY_FUNCTION__ << state;
    ++setStateCount;
    setStateParam = state;

    QTimer::singleShot(10000, this, SLOT(switchMe()));
}

void DummyInputMethod::switchMe()
{
    qDebug() << __PRETTY_FUNCTION__;
    emit pluginSwitchRequired(M::SwitchForward);
}

void DummyInputMethod::switchMe(const QString &name)
{
    qDebug() << __PRETTY_FUNCTION__;
    emit pluginSwitchRequired(name);
}

void DummyInputMethod::switchContext(M::InputMethodSwitchDirection direction,
        bool enableAnimation)
{
    ++switchContextCallCount;
    directionParam = direction;
    enableAnimationParam = enableAnimation;
}

