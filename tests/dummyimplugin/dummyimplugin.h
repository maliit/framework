#ifndef DUMMYIMPLUGIN_H
#define DUMMYIMPLUGIN_H

#include <QObject>
#include <QWidget>

#include "minputmethodplugin.h"


//! Dummy input method plugin for ut_mimpluginloader
class DummyImPlugin: public QObject,
    public Maliit::Server::InputMethodPlugin
{
    Q_OBJECT
    Q_INTERFACES(Maliit::Server::InputMethodPlugin)

public:
    DummyImPlugin();

    //! \reimp
    virtual QString name() const;

    virtual MAbstractInputMethod *createInputMethod(MAbstractInputMethodHost *host);

    virtual QSet<MInputMethod::HandlerState> supportedStates() const;
    //! \reimp_end

public:
    QSet<MInputMethod::HandlerState> allowedStates;
};

#endif
