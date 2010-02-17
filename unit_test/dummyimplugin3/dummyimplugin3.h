#ifndef DUMMYIMPLUGIN3_H
#define DUMMYIMPLUGIN3_H

#include <QObject>
#include "duiinputmethodplugin.h"


//! Dummy input method plugin for ut_duiimpluginloader
class DummyImPlugin3: public QObject,
    public DuiInputMethodPlugin
{
    Q_OBJECT
    Q_INTERFACES(DuiInputMethodPlugin)

public:
    DummyImPlugin3();

    //! \reimp
    virtual QString name() const;

    virtual QStringList languages() const;

    virtual DuiInputMethodBase *createInputMethod(DuiInputContextConnection *icConnection);
    //! \reimp_end

public:
    int setStateCount;
    QList<DuiIMHandlerState> setStateParam;
};

#endif
