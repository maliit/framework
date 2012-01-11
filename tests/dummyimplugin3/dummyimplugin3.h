#ifndef DUMMYIMPLUGIN3_H
#define DUMMYIMPLUGIN3_H

#include <QObject>
#include <QWidget>

#include "minputmethodplugin.h"

//! Dummy input method plugin for ut_mimpluginloader
class DummyImPlugin3: public QObject,
    public Maliit::Plugins::InputMethodPlugin
{
    Q_OBJECT
    Q_INTERFACES(Maliit::Plugins::InputMethodPlugin)

public:
    DummyImPlugin3();

    //! \reimp
    virtual QString name() const;

    virtual MAbstractInputMethod *createInputMethod(MAbstractInputMethodHost *host,
                                                    QWidget *mainWindow);

    virtual QSet<Maliit::HandlerState> supportedStates() const;
    //! \reimp_end

public:
    int setStateCount;
    QList<Maliit::HandlerState> setStateParam;
    QSet<Maliit::HandlerState> allowedStates;
};

#endif
