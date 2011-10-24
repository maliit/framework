#ifndef MIMXAPPLICATION_H
#define MIMXAPPLICATION_H

#include "mpassthruwindow.h"
#include "mimremotewindow.h"
#include "mimxextension.h"

#include <memory>
#include <tr1/functional>

#include <QApplication>

class MImRotationAnimation;

class MImXApplication : public QApplication
{
    Q_OBJECT
public:
    //! Walks over widget hierarchy, if used with
    //! MImXApplication::visitWidgetHierarchy. Return true if children of
    //! current widget shall be visited, too.
    typedef std::tr1::function<bool (QWidget *)> WidgetVisitor;

    MImXApplication(int &argc, char** argv);
    virtual ~MImXApplication();
    static MImXApplication *instance();

    const MImXCompositeExtension& compositeExtension() { return mCompositeExtension; }
    const MImXDamageExtension& damageExtension() { return mDamageExtension; }

    //! Requires a valid remoteWinId and a valid passThruWindow before it'll
    //! start to work.
    //! \sa setTransientHint(), setPassThruWindow();
    bool x11EventFilter(XEvent *ev);

    virtual bool selfComposited() const;
    bool manualRedirection() const;
    bool bypassWMHint() const;
    bool unconditionalShow() const;

    //! Flag that is used to sync between MAbstractInputMethodHost and
    //! rotation animation in order to capture the VKB without
    //! self-composited background drawn.
    void setSuppressBackground(bool suppress);

    QWidget *passThruWindow() const;
    QWidget *pluginsProxyWidget() const;

    virtual const QPixmap &remoteWindowPixmap();

    void visitWidgetHierarchy(WidgetVisitor visitor, QWidget *widget);

    //! For unittesting purposes
    MImRemoteWindow *remoteWindow() const;

public Q_SLOTS:
    void setTransientHint(WId remoteWinId);

    void configureWidgetsForCompositing();

Q_SIGNALS:
    //! This signal is emitted when remote window is changed.
    //! Parameter can be 0 if window is unmapped.
    void remoteWindowChanged(MImRemoteWindow *newWindow);

    void remoteWindowGone();

private Q_SLOTS:
    void finalize();

    void updatePassThruWindow(const QRegion &region);

    void appOrientationAboutToChange(int toAngle);
    void appOrientationChangeFinished(int toAngle);

private:
    void parseArguments(int &argc, char** argv);

    void handleTransientEvents(XEvent *ev);
    void handleRemoteWindowEvents(XEvent *ev);
    void handlePassThruMapEvent(XEvent *ev);

    MImXCompositeExtension mCompositeExtension;
    MImXDamageExtension mDamageExtension;

    bool mSelfComposited;
    bool mManualRedirection;
    bool mBypassWMHint;
    bool mBackgroundSuppressed;
    bool mUnconditionalShow;

    std::auto_ptr<MPassThruWindow> mPassThruWindow;
    std::auto_ptr<QWidget> mPluginsProxyWidget;
    std::auto_ptr<MImRemoteWindow> mRemoteWindow;
    std::auto_ptr<MImRotationAnimation> mRotationAnimation;

    friend class Ut_PassthroughServer;
};

#endif // MIMXAPPLICATION_H
