#ifndef DUMMYINPUTMETHOD_H
#define DUMMYINPUTMETHOD_H

#include <minputmethodbase.h>
#include <QSet>

#include "mtoolbardata.h"
#include "minputmethodnamespace.h"

class DummyInputMethod : public MInputMethodBase
{
    Q_OBJECT

public:

    DummyInputMethod(MAbstractInputMethodHost *imHost);

    //! \reimp
    virtual void setState(const QSet<MInputMethod::HandlerState> &state);
    virtual void switchContext(MInputMethod::SwitchDirection direction,
                               bool enableAnimation);
    virtual QList<MInputMethodBase::MInputMethodSubView> subViews(MInputMethod::HandlerState state
                                                                   = MInputMethod::OnScreen) const;
    virtual void setActiveSubView(const QString &,
                                  MInputMethod::HandlerState state = MInputMethod::OnScreen);
    virtual QString activeSubView(MInputMethod::HandlerState state = MInputMethod::OnScreen) const;
    virtual void setToolbar(QSharedPointer<const MToolbarData> toolbar);
    //! \reimp_end

public:
    int setStateCount;
    QSet<MInputMethod::HandlerState> setStateParam;

    int switchContextCallCount;
    MInputMethod::SwitchDirection directionParam;
    bool enableAnimationParam;
    QSharedPointer<const MToolbarData> toolbarParam;

public slots:
    void switchMe();
    void switchMe(const QString &name);

private:
    QList<MInputMethodBase::MInputMethodSubView> sViews;
    QString activeSView;
};

#endif

