#ifndef DUMMYINPUTMETHOD3_H
#define DUMMYINPUTMETHOD3_H

#include <maliit/plugins/abstractinputmethod.h>
#include <maliit/plugins/abstractpluginsetting.h>
#include <QSet>
#include <QWidget>

class DummyInputMethod3 : public MAbstractInputMethod
{
    Q_OBJECT

public:

    DummyInputMethod3(MAbstractInputMethodHost *host);

    //! \reimp
    virtual void setState(const QSet<Maliit::HandlerState> &state);
    virtual void switchContext(Maliit::SwitchDirection direction, bool enableAnimation);
    virtual QList<MAbstractInputMethod::MInputMethodSubView> subViews(Maliit::HandlerState state
                                                                   = Maliit::OnScreen) const;
    virtual void setActiveSubView(const QString &,
                                  Maliit::HandlerState state = Maliit::OnScreen);
    virtual QString activeSubView(Maliit::HandlerState state = Maliit::OnScreen) const;
    virtual void show();
    //! \reimp_end

public:
    int setStateCount;
    QSet<Maliit::HandlerState> setStateParam;

    int switchContextCallCount;
    Maliit::SwitchDirection directionParam;
    bool enableAnimationParam;

    QVariant localSettingValue;
    QScopedPointer<Maliit::Plugins::AbstractPluginSetting> setting;

Q_SIGNALS:
    void showCalled();

private:
    Q_SLOT void handleSettingChanged();

    void addSubView(const QString &id, const QString &title);

    QList<MAbstractInputMethod::MInputMethodSubView> sViews;
    QString activeSView;
};

#endif

