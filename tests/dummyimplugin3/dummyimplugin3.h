#ifndef DUMMYIMPLUGIN3_H
#define DUMMYIMPLUGIN3_H

#include <QObject>
#include <QWidget>

#include <maliit/plugins/inputmethodplugin.h>

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

    virtual MAbstractInputMethod *createInputMethod(MAbstractInputMethodHost *host);

    virtual QSet<MInputMethod::HandlerState> supportedStates() const;
    //! \reimp_end

public:
    int setStateCount;
    QList<MInputMethod::HandlerState> setStateParam;
    QSet<MInputMethod::HandlerState> allowedStates;
};

#endif
