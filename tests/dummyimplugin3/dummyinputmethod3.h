#ifndef DUMMYINPUTMETHOD3_H
#define DUMMYINPUTMETHOD3_H

#include <mabstractinputmethod.h>
#include <QSet>
#include <QWidget>

class DummyInputMethod3 : public MAbstractInputMethod
{
    Q_OBJECT

public:

    DummyInputMethod3(MAbstractInputMethodHost *host,
                      QWidget *mainWindow);

    //! \reimp
    virtual void setState(const QSet<MInputMethod::HandlerState> &state);
    virtual void switchContext(MInputMethod::SwitchDirection direction, bool enableAnimation);
    virtual QList<MAbstractInputMethod::MInputMethodSubView> subViews(MInputMethod::HandlerState state
                                                                   = MInputMethod::OnScreen) const;
    virtual void setActiveSubView(const QString &,
                                  MInputMethod::HandlerState state = MInputMethod::OnScreen);
    virtual QString activeSubView(MInputMethod::HandlerState state = MInputMethod::OnScreen) const;
    virtual void show();
    //! \reimp_end

public:
    int setStateCount;
    QSet<MInputMethod::HandlerState> setStateParam;

    int switchContextCallCount;
    MInputMethod::SwitchDirection directionParam;
    bool enableAnimationParam;

Q_SIGNALS:
    void showCalled();

private:
    QList<MAbstractInputMethod::MInputMethodSubView> sViews;
    QString activeSView;
};

#endif

