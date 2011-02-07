#ifndef DUMMYINPUTMETHOD3_H
#define DUMMYINPUTMETHOD3_H

#include <mabstractinputmethod.h>
#include <QSet>
#include <QWidget>

#include "mtoolbardata.h"

class DummyInputMethod3 : public MAbstractInputMethod
{
    Q_OBJECT

public:

    DummyInputMethod3(MAbstractInputMethodHost *host,
                      QWidget *mainWindow);

    //! \reimp
    virtual void setState(const QSet<MInputMethod::HandlerState> &state);
    virtual void switchContext(MInputMethod::SwitchDirection direction, bool enableAnimation);
    virtual QList<MAbstractInputMethod::MInputMethodSubView> subViews(MInputMethod::HandlerState state
                                                                   = MInputMethod::OnScreen) const;
    virtual void setActiveSubView(const QString &,
                                  MInputMethod::HandlerState state = MInputMethod::OnScreen);
    virtual QString activeSubView(MInputMethod::HandlerState state = MInputMethod::OnScreen) const;
    virtual void show();
    virtual void setToolbar(QSharedPointer<const MToolbarData> toolbar);
    //! \reimp_end

public:
    int setStateCount;
    QSet<MInputMethod::HandlerState> setStateParam;

    int switchContextCallCount;
    MInputMethod::SwitchDirection directionParam;
    bool enableAnimationParam;
    QSharedPointer<const MToolbarData> toolbarParam;

private:
    QList<MAbstractInputMethod::MInputMethodSubView> sViews;
    QString activeSView;
};

#endif

