#ifndef DUMMYIMPLUGIN3_H
#define DUMMYIMPLUGIN3_H

#include <QObject>

#include <maliit/plugins/inputmethodplugin.h>

//! Dummy input method plugin for ut_mimpluginloader
class DummyImPlugin3: public QObject,
    public Maliit::Plugins::InputMethodPlugin
{
    Q_OBJECT
    Q_INTERFACES(Maliit::Plugins::InputMethodPlugin)
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_PLUGIN_METADATA(IID  "org.maliit.tests.dummyimplugin3"
                      FILE "dummyimplugin3.json")
#endif

public:
    DummyImPlugin3();

    //! \reimp
    virtual QString name() const;

    virtual MAbstractInputMethod *createInputMethod(MAbstractInputMethodHost *host);

    virtual QSet<Maliit::HandlerState> supportedStates() const;
    //! \reimp_end

public:
    int setStateCount;
    QList<Maliit::HandlerState> setStateParam;
    QSet<Maliit::HandlerState> allowedStates;
};

#endif
