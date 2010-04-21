#ifndef DUMMYINPUTMETHOD3_H
#define DUMMYINPUTMETHOD3_H

#include <minputmethodbase.h>
#include <QSet>

class DummyInputMethod3 : public MInputMethodBase
{
    Q_OBJECT

public:

    DummyInputMethod3(MInputContextConnection *connection);

    //! \reimp
    virtual void setState(const QSet<MIMHandlerState> &state);
    virtual void switchContext(M::InputMethodSwitchDirection direction, bool enableAnimation);
    //! \reimp_end

public:
    int setStateCount;
    QSet<MIMHandlerState> setStateParam;

    int switchContextCallCount;
    M::InputMethodSwitchDirection directionParam;
    bool enableAnimationParam;
};

#endif

