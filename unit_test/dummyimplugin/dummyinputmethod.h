#ifndef DUMMYINPUTMETHOD_H
#define DUMMYINPUTMETHOD_H

#include <minputmethodbase.h>
#include <QList>

class DummyInputMethod : public MInputMethodBase
{
    Q_OBJECT

public:

    DummyInputMethod(MInputContextConnection *connection);

    //! \reimp
    virtual void setState(const QList<MIMHandlerState> &state);
    //! \reimp_end

public:
    int setStateCount;
    QList<MIMHandlerState> setStateParam;
};

#endif

