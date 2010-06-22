#ifndef DUMMYINPUTMETHOD_H
#define DUMMYINPUTMETHOD_H

#include <minputmethodbase.h>
#include <QSet>

class DummyInputMethod : public MInputMethodBase
{
    Q_OBJECT

public:

    DummyInputMethod(MInputContextConnection *connection);

    //! \reimp
    virtual void setState(const QSet<MIMHandlerState> &state);
    virtual void switchContext(M::InputMethodSwitchDirection direction, bool enableAnimation);
    virtual QList<MInputMethodBase::MInputMethodSubView> subViews(MIMHandlerState state = OnScreen) const;
    virtual void setActiveSubView(const QString &, MIMHandlerState state = OnScreen);
    virtual QString activeSubView(MIMHandlerState state = OnScreen) const;
    //! \reimp_end

public:
    int setStateCount;
    QSet<MIMHandlerState> setStateParam;

    int switchContextCallCount;
    M::InputMethodSwitchDirection directionParam;
    bool enableAnimationParam;

public slots:
    void switchMe();
    void switchMe(const QString &name);

private:
    QList<MInputMethodBase::MInputMethodSubView> sViews;
    QString activeSView;
};

#endif

