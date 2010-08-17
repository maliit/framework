#include "dummyinputmethod3.h"
#include <QDebug>

DummyInputMethod3::DummyInputMethod3(MInputContextConnection *connection)
    : MInputMethodBase(connection),
      setStateCount(0),
      switchContextCallCount(0),
      directionParam(M::SwitchUndefined),
      enableAnimationParam(false)
{
    MInputMethodBase::MInputMethodSubView sv1;
    sv1.subViewId = "dummyim3sv1";
    sv1.subViewTitle = "dummyim3sv1";
    sViews.append(sv1);

    MInputMethodBase::MInputMethodSubView sv2;
    sv2.subViewId = "dummyim3sv2";
    sv2.subViewTitle = "dummyim3sv2";
    sViews.append(sv2);

    activeSView = "dummyim3sv1";
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

QList<MInputMethodBase::MInputMethodSubView> DummyInputMethod3::subViews(MIMHandlerState state) const
{
    qDebug() << __PRETTY_FUNCTION__;
    QList<MInputMethodBase::MInputMethodSubView> svs;
    if (state == OnScreen) {
        svs = sViews;
    }
    return svs;
}

void DummyInputMethod3::setActiveSubView(const QString &sVId, MIMHandlerState state)
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

QString DummyInputMethod3::activeSubView(MIMHandlerState state) const
{
    qDebug() << __PRETTY_FUNCTION__;
    if (state == OnScreen)
        return activeSView;
    else
        return QString();
}

void DummyInputMethod3::show()
{
    emit regionUpdated(QRegion(0, 0, 100, 100));
}

