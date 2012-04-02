#ifndef DUMMYIMPLUGIN2_H
#define DUMMYIMPLUGIN2_H

#include <QObject>
#include <QWidget>
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

    virtual MAbstractInputMethod *createInputMethod(MAbstractInputMethodHost *host,
                                                    QWidget *mainWindow);

    virtual MAbstractInputMethodSettings *createInputMethodSettings();

    virtual QSet<Maliit::HandlerState> supportedStates() const;
    //! \reimp_end
};

#endif
