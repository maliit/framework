#ifndef DUMMYIMPLUGIN3_H
#define DUMMYIMPLUGIN3_H

#include <QObject>
#include <QWidget>

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

    virtual MAbstractInputMethod *createInputMethod(MAbstractInputMethodHost *host,
                                                    QWidget *mainWindow);

    virtual MAbstractInputMethodSettings *createInputMethodSettings();

    virtual QSet<MInputMethod::HandlerState> supportedStates() const;
    //! \reimp_end

public:
    int setStateCount;
    QList<MInputMethod::HandlerState> setStateParam;
    QSet<MInputMethod::HandlerState> allowedStates;
};

#endif
