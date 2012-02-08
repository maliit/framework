#ifndef DUMMYINPUTMETHOD_H
#define DUMMYINPUTMETHOD_H

#include <maliit/plugins/abstractinputmethod.h>
#include <QSet>
#include <QWidget>

class DummyInputMethod : public MAbstractInputMethod
{
    Q_OBJECT

public:

    DummyInputMethod(MAbstractInputMethodHost *host);

    //! \reimp
    virtual void setState(const QSet<Maliit::HandlerState> &state);
    virtual void switchContext(Maliit::SwitchDirection direction,
                               bool enableAnimation);
    virtual QList<MAbstractInputMethod::MInputMethodSubView> subViews(Maliit::HandlerState state
                                                                   = Maliit::OnScreen) const;
    virtual void setActiveSubView(const QString &,
                                  Maliit::HandlerState state = Maliit::OnScreen);
    virtual QString activeSubView(Maliit::HandlerState state = Maliit::OnScreen) const;
    //! \reimp_end

public:
    int setStateCount;
    QSet<Maliit::HandlerState> setStateParam;

    int switchContextCallCount;
    Maliit::SwitchDirection directionParam;
    bool enableAnimationParam;

    int pluginsChangedSignalCount;

public Q_SLOTS:
    void switchMe();
    void switchMe(const QString &name);

private Q_SLOTS:
    void onPluginsChange();

private:
    QList<MAbstractInputMethod::MInputMethodSubView> sViews;
    QString activeSView;
};

#endif

