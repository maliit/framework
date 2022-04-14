#ifndef DUMMYIMPLUGIN2_H
#define DUMMYIMPLUGIN2_H

#include <QObject>

#include <maliit/plugins/inputmethodplugin.h>

//! Dummy input method plugin for ut_mimpluginloader
class DummyImPlugin2: public QObject,
    public Maliit::Plugins::InputMethodPlugin
{
    Q_OBJECT
    Q_INTERFACES(Maliit::Plugins::InputMethodPlugin)
    Q_PLUGIN_METADATA(IID  "org.maliit.tests.dummyimplugin2"
                      FILE "dummyimplugin2.json")

public:
    //! \reimp
    virtual QString name() const;

    virtual MAbstractInputMethod *createInputMethod(MAbstractInputMethodHost *host);

    virtual QSet<Maliit::HandlerState> supportedStates() const;
    //! \reimp_end
};

#endif
