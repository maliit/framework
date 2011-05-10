#ifndef DUMMYINPUTMETHOD_H
#define DUMMYINPUTMETHOD_H

#include <mabstractinputmethod.h>
#include <QSet>
#include <QWidget>

#include "mtoolbardata.h"
#include "minputmethodnamespace.h"

class DummyInputMethod : public MAbstractInputMethod
{
    Q_OBJECT

public:

    DummyInputMethod(MAbstractInputMethodHost *host,
                     QWidget *mainWindow);

    //! \reimp
    virtual void setState(const QSet<MInputMethod::HandlerState> &state);
    virtual void switchContext(MInputMethod::SwitchDirection direction,
                               bool enableAnimation);
    virtual QList<MAbstractInputMethod::MInputMethodSubView> subViews(MInputMethod::HandlerState state
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

    int pluginsChangedSignalCount;

public slots:
    void switchMe();
    void switchMe(const QString &name);

private slots:
    void onPluginsChange();

private:
    QList<MAbstractInputMethod::MInputMethodSubView> sViews;
    QString activeSView;
};

#endif

