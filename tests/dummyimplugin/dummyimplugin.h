#ifndef DUMMYIMPLUGIN_H
#define DUMMYIMPLUGIN_H

#include <QObject>
#include <QWidget>

#include <maliit/plugins/inputmethodplugin.h>


//! Dummy input method plugin for ut_mimpluginloader
class DummyImPlugin: public QObject,
    public Maliit::Plugins::InputMethodPlugin
{
    Q_OBJECT
    Q_INTERFACES(Maliit::Plugins::InputMethodPlugin)

public:
    DummyImPlugin();

    //! \reimp
    virtual QString name() const;

    virtual MAbstractInputMethod *createInputMethod(MAbstractInputMethodHost *host);

    virtual QSet<Maliit::HandlerState> supportedStates() const;
    //! \reimp_end

public:
    QSet<Maliit::HandlerState> allowedStates;
};

#endif
