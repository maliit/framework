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
    MInputMethodBase::MInputMethodSubView sv1;
    sv1.subViewId = "dummyimsv1";
    sv1.subViewTitle = "dummimysv1";
    sViews.append(sv1);

    MInputMethodBase::MInputMethodSubView sv2;
    sv2.subViewId = "dummyimsv2";
    sv2.subViewTitle = "dummyimsv2";
    sViews.append(sv2);

    activeSView = "dummyimsv1";
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

QList<MInputMethodBase::MInputMethodSubView> DummyInputMethod::subViews(MIMHandlerState state) const
{
    qDebug() << __PRETTY_FUNCTION__;
    QList<MInputMethodBase::MInputMethodSubView> svs;
    if (state == OnScreen) {
        svs = sViews;
    }
    return svs;
}

void DummyInputMethod::setActiveSubView(const QString &sVId, MIMHandlerState state)
{
    qDebug() << __PRETTY_FUNCTION__;
    if (state == OnScreen) {
        foreach (const MInputMethodBase::MInputMethodSubView &sv, sViews) {
            if (sv.subViewId == sVId) {
                activeSView = sVId;
            }
        }
    }
}

QString DummyInputMethod::activeSubView(MIMHandlerState state) const
{
    qDebug() << __PRETTY_FUNCTION__;
    if (state == OnScreen)
        return activeSView;
    else
        return QString();
}

void DummyInputMethod::setToolbar(QSharedPointer<const MToolbarData> toolbar)
{
    toolbarParam = toolbar;
}

