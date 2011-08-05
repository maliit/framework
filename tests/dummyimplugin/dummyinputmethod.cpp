#include "dummyinputmethod.h"
#include <QDebug>
#include <QTimer>

#include "mabstractinputmethodhost.h"

DummyInputMethod::DummyInputMethod(MAbstractInputMethodHost *host,
                                   QWidget *mainWindow)
    : MAbstractInputMethod(host, mainWindow),
      setStateCount(0),
      switchContextCallCount(0),
      directionParam(MInputMethod::SwitchUndefined),
      enableAnimationParam(false),
      pluginsChangedSignalCount(0)
{
    MAbstractInputMethod::MInputMethodSubView sv1;
    sv1.subViewId = "dummyimsv1";
    sv1.subViewTitle = "dummimysv1";
    sViews.append(sv1);

    MAbstractInputMethod::MInputMethodSubView sv2;
    sv2.subViewId = "dummyimsv2";
    sv2.subViewTitle = "dummyimsv2";
    sViews.append(sv2);

    MAbstractInputMethod::MInputMethodSubView sv3;
    sv3.subViewId = "dummyimsv3";
    sv3.subViewTitle = "dummyimsv3";
    sViews.append(sv3);

    activeSView = "dummyimsv1";

    connect(host, SIGNAL(pluginsChanged()),
            this, SLOT(onPluginsChange()));
}

void DummyInputMethod::setState(const QSet<MInputMethod::HandlerState> &state)
{
    qDebug() << __PRETTY_FUNCTION__ << state;
    ++setStateCount;
    setStateParam = state;

    QTimer::singleShot(10000, this, SLOT(switchMe()));
}

void DummyInputMethod::switchMe()
{
    qDebug() << __PRETTY_FUNCTION__;
    inputMethodHost()->switchPlugin(MInputMethod::SwitchForward);
}

void DummyInputMethod::switchMe(const QString &name)
{
    qDebug() << __PRETTY_FUNCTION__;
    inputMethodHost()->switchPlugin(name);
}

void DummyInputMethod::onPluginsChange()
{
    ++pluginsChangedSignalCount;
}

void DummyInputMethod::switchContext(MInputMethod::SwitchDirection direction, bool enableAnimation)
{
    ++switchContextCallCount;
    directionParam = direction;
    enableAnimationParam = enableAnimation;
}

QList<MAbstractInputMethod::MInputMethodSubView>
DummyInputMethod::subViews(MInputMethod::HandlerState state) const
{
    qDebug() << __PRETTY_FUNCTION__;
    QList<MAbstractInputMethod::MInputMethodSubView> svs;
    if (state == MInputMethod::OnScreen) {
        svs = sViews;
    }
    return svs;
}

void DummyInputMethod::setActiveSubView(const QString &sVId, MInputMethod::HandlerState state)
{
    qDebug() << __PRETTY_FUNCTION__;
    if (state == MInputMethod::OnScreen) {
        foreach (const MAbstractInputMethod::MInputMethodSubView &sv, sViews) {
            if (sv.subViewId == sVId) {
                activeSView = sVId;
            }
        }
    }
}

QString DummyInputMethod::activeSubView(MInputMethod::HandlerState state) const
{
    qDebug() << __PRETTY_FUNCTION__;
    if (state == MInputMethod::OnScreen)
        return activeSView;
    else
        return QString();
}

void DummyInputMethod::setToolbar(QSharedPointer<const MToolbarData> toolbar)
{
    toolbarParam = toolbar;
}

