#ifndef DUMMYIMPLUGIN2_H
#define DUMMYIMPLUGIN2_H

#include <QObject>
#include <QWidget>

#include <maliit/plugins/inputmethodplugin.h>

//! Dummy input method plugin for ut_mimpluginloader
class DummyImPlugin2: public QObject,
    public Maliit::Plugins::InputMethodPlugin
{
    Q_OBJECT
    Q_INTERFACES(Maliit::Plugins::InputMethodPlugin)

public:
    //! \reimp
    virtual QString name() const;

    virtual MAbstractInputMethod *createInputMethod(MAbstractInputMethodHost *host,
                                                    QWidget *mainWindow);

    virtual QSet<MInputMethod::HandlerState> supportedStates() const;
    //! \reimp_end
};

#endif
