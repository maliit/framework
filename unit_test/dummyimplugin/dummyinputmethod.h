#ifndef DUMMYINPUTMETHOD_H
#define DUMMYINPUTMETHOD_H

#include <duiinputmethodbase.h>
#include <QList>

class DummyInputMethod : public DuiInputMethodBase
{
    Q_OBJECT

public:

    DummyInputMethod(DuiInputContextConnection *connection);

    //! \reimp
    virtual void setState(const QList<DuiIMHandlerState> &state);
    //! \reimp_end

public:
    int setStateCount;
    QList<DuiIMHandlerState> setStateParam;
};

#endif

