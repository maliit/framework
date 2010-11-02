#ifndef DUMMYIMPLUGIN_H
#define DUMMYIMPLUGIN_H

#include <QObject>
#include "minputmethodplugin.h"


//! Dummy input method plugin for ut_mimpluginloader
class DummyImPlugin: public QObject,
    public MInputMethodPlugin
{
    Q_OBJECT
    Q_INTERFACES(MInputMethodPlugin)

public:
    DummyImPlugin();

    //! \reimp
    virtual QString name() const;

    virtual QStringList languages() const;

    virtual MAbstractInputMethod *createInputMethod(MAbstractInputMethodHost *imHost);

    virtual MInputMethodSettingsBase *createInputMethodSettings();

    virtual QSet<MInputMethod::HandlerState> supportedStates() const;
    //! \reimp_end

public:
    QSet<MInputMethod::HandlerState> allowedStates;
};

#endif
