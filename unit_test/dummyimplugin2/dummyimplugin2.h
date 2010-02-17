#ifndef DUMMYIMPLUGIN2_H
#define DUMMYIMPLUGIN2_H

#include <QObject>
#include "duiinputmethodplugin.h"


//! Dummy input method plugin for ut_duiimpluginloader
class DummyImPlugin2: public QObject,
    public DuiInputMethodPlugin
{
    Q_OBJECT
    Q_INTERFACES(DuiInputMethodPlugin)

public:
    //! \reimp
    virtual QString name() const;

    virtual QStringList languages() const;

    virtual DuiInputMethodBase *createInputMethod(DuiInputContextConnection *icConnection);
    //! \reimp_end
};

#endif
