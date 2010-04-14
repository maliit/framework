#ifndef DUMMYINPUTMETHOD3_H
#define DUMMYINPUTMETHOD3_H

#include <minputmethodbase.h>
#include <QList>

class DummyInputMethod3 : public MInputMethodBase
{
    Q_OBJECT

public:

    DummyInputMethod3(MInputContextConnection *connection);

    //! \reimp
    virtual void setState(const QList<MIMHandlerState> &state);
    //! \reimp_end

public:
    int setStateCount;
    QList<MIMHandlerState> setStateParam;
};

#endif

