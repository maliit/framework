#include "dummyinputmethod3.h"
#include <maliit/plugins/abstractinputmethodhost.h>

#include <QDebug>
#include <QRegion>

DummyInputMethod3::DummyInputMethod3(MAbstractInputMethodHost *host,
                                     QWidget *mainWindow)
    : MAbstractInputMethod(host, mainWindow),
      setStateCount(0),
      switchContextCallCount(0),
      directionParam(Maliit::SwitchUndefined),
      enableAnimationParam(false)
{
    MAbstractInputMethod::MInputMethodSubView sv1;
    sv1.subViewId = "dummyim3sv1";
    sv1.subViewTitle = "dummyim3sv1";
    sViews.append(sv1);

    MAbstractInputMethod::MInputMethodSubView sv2;
    sv2.subViewId = "dummyim3sv2";
    sv2.subViewTitle = "dummyim3sv2";
    sViews.append(sv2);

    activeSView = "dummyim3sv1";
}

void DummyInputMethod3::setState(const QSet<Maliit::HandlerState> &state)
{
    qDebug() << __PRETTY_FUNCTION__ << state;
    ++setStateCount;
    setStateParam = state;
}

void DummyInputMethod3::switchContext(Maliit::SwitchDirection direction,
                              bool enableAnimation)
{
    ++switchContextCallCount;
    directionParam = direction;
    enableAnimationParam = enableAnimation;
}

QList<MAbstractInputMethod::MInputMethodSubView>
DummyInputMethod3::subViews(Maliit::HandlerState state) const
{
    qDebug() << __PRETTY_FUNCTION__;
    QList<MAbstractInputMethod::MInputMethodSubView> svs;
    if (state == Maliit::OnScreen) {
        svs = sViews;
    }
    return svs;
}

void DummyInputMethod3::setActiveSubView(const QString &sVId, Maliit::HandlerState state)
{
    qDebug() << __PRETTY_FUNCTION__;
    if (state == Maliit::OnScreen) {
        Q_FOREACH (const MAbstractInputMethod::MInputMethodSubView &sv, sViews) {
            if (sv.subViewId == sVId) {
                activeSView = sVId;
            }
        }
    }
}

QString DummyInputMethod3::activeSubView(Maliit::HandlerState state) const
{
    qDebug() << __PRETTY_FUNCTION__;
    if (state == Maliit::OnScreen)
        return activeSView;
    else
        return QString();
}

void DummyInputMethod3::show()
{
    inputMethodHost()->setScreenRegion(QRegion(0, 0, 100, 100));
    Q_EMIT showCalled();
}
