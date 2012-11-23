#include "dummyinputmethod3.h"
#include <maliit/plugins/abstractinputmethodhost.h>

#include <QDebug>
#include <QRegion>

void DummyInputMethod3::addSubView(const QString &id, const QString &title)
{
    MAbstractInputMethod::MInputMethodSubView sv;
    sv.subViewId = id;
    sv.subViewTitle = title;
    sViews.append(sv);
}

DummyInputMethod3::DummyInputMethod3(MAbstractInputMethodHost *host)
    : MAbstractInputMethod(host),
      setStateCount(0),
      switchContextCallCount(0),
      directionParam(Maliit::SwitchUndefined),
      enableAnimationParam(false)
{
    addSubView("dummyim3sv1", "dummyim3sv1");
    addSubView("dummyim3sv2", "dummyim3sv2");
    addSubView("en_gb", "en_gb");
    addSubView("es", "es");
    addSubView("fr_fr", "fr_fr");

    activeSView = "dummyim3sv1";

    // Register setting
    QVariantMap settingAttributes;

    settingAttributes[Maliit::SettingEntryAttributes::defaultValue] = "Test";

    setting.reset(host->registerPluginSetting("setting", QT_TR_NOOP("Example setting"),
                                              Maliit::StringType, settingAttributes));

    connect(setting.data(), SIGNAL(valueChanged()),
            this,           SLOT(handleSettingChanged()));
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

void DummyInputMethod3::handleSettingChanged()
{
    localSettingValue = setting->value();
}
