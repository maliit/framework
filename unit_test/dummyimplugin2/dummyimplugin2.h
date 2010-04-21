#ifndef DUMMYIMPLUGIN2_H
#define DUMMYIMPLUGIN2_H

#include <QObject>
#include "minputmethodplugin.h"


//! Dummy input method plugin for ut_mimpluginloader
class DummyImPlugin2: public QObject,
    public MInputMethodPlugin
{
    Q_OBJECT
    Q_INTERFACES(MInputMethodPlugin)

public:
    //! \reimp
    virtual QString name() const;

    virtual QStringList languages() const;

    virtual MInputMethodBase *createInputMethod(MInputContextConnection *icConnection);

    virtual QSet<MIMHandlerState> supportedStates() const;
    //! \reimp_end
};

#endif
