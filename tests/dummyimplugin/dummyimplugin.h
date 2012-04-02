#ifndef DUMMYIMPLUGIN_H
#define DUMMYIMPLUGIN_H

#include <QObject>
#include <QWidget>

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

    virtual MAbstractInputMethod *createInputMethod(MAbstractInputMethodHost *host,
                                                    QWidget *mainWindow);

    virtual MAbstractInputMethodSettings *createInputMethodSettings();

    virtual QSet<Maliit::HandlerState> supportedStates() const;
    //! \reimp_end

public:
    QSet<Maliit::HandlerState> allowedStates;
};

#endif
