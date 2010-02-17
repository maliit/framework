#ifndef DUMMYINPUTMETHOD3_H
#define DUMMYINPUTMETHOD3_H

#include <duiinputmethodbase.h>
#include <QList>

class DummyInputMethod3 : public DuiInputMethodBase
{
    Q_OBJECT

public:

    DummyInputMethod3(DuiInputContextConnection *connection);

    //! \reimp
    virtual void setState(const QList<DuiIMHandlerState> &state);
    //! \reimp_end

public:
    int setStateCount;
    QList<DuiIMHandlerState> setStateParam;
};

#endif

