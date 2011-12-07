#ifndef DUMMYIMPLUGIN3_H
#define DUMMYIMPLUGIN3_H

#include <QObject>
#include <QWidget>

#include "minputmethodplugin.h"

//! Dummy input method plugin for ut_mimpluginloader
class DummyImPlugin3: public QObject,
    public Maliit::Server::InputMethodPlugin
{
    Q_OBJECT
    Q_INTERFACES(Maliit::Server::InputMethodPlugin)

public:
    DummyImPlugin3();

    //! \reimp
    virtual QString name() const;

    virtual MAbstractInputMethod *createInputMethod(MAbstractInputMethodHost *host,
                                                    std::tr1::shared_ptr<Maliit::Server::SurfaceFactory> factory);

    virtual QSet<MInputMethod::HandlerState> supportedStates() const;
    //! \reimp_end

public:
    int setStateCount;
    QList<MInputMethod::HandlerState> setStateParam;
    QSet<MInputMethod::HandlerState> allowedStates;
};

#endif
