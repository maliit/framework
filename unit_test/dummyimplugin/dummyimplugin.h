#ifndef DUMMYIMPLUGIN_H
#define DUMMYIMPLUGIN_H

#include <QObject>
#include "duiinputmethodplugin.h"


//! Dummy input method plugin for ut_duiimpluginloader
class DummyImPlugin: public QObject,
    public DuiInputMethodPlugin
{
    Q_OBJECT
    Q_INTERFACES(DuiInputMethodPlugin)

public:
    DummyImPlugin();

    //! \reimp
    virtual QString name() const;

    virtual QStringList languages() const;

    virtual DuiInputMethodBase *createInputMethod(DuiInputContextConnection *icConnection);
    //! \reimp_end
};

#endif
