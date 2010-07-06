#ifndef DUMMYIMPLUGIN3_H
#define DUMMYIMPLUGIN3_H

#include <QObject>
#include "minputmethodplugin.h"

//! Dummy input method plugin for ut_mimpluginloader
class DummyImPlugin3: public QObject,
    public MInputMethodPlugin
{
    Q_OBJECT
    Q_INTERFACES(MInputMethodPlugin)

public:
    DummyImPlugin3();

    //! \reimp
    virtual QString name() const;

    virtual QStringList languages() const;

    virtual MInputMethodBase *createInputMethod(MInputContextConnection *icConnection);

    virtual MInputMethodSettingsBase *createInputMethodSettings();

    virtual QSet<MIMHandlerState> supportedStates() const;
    //! \reimp_end

public:
    int setStateCount;
    QList<MIMHandlerState> setStateParam;
    QSet<MIMHandlerState> allowedStates;
};

#endif
