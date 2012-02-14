#include "mimxapplication.h"
#include "mimpluginsproxywidget.h"
#include "mimrotationanimation.h"
#include "mimserveroptions.h"

#include <QDebug>

#ifdef HAVE_MEEGOGRAPHICSSYSTEM
#include <QtMeeGoGraphicsSystemHelper/QMeeGoGraphicsSystemHelper>
#endif

#include <deque>

#include <X11/Xlib.h>

namespace
{
    bool configureForCompositing(QWidget *w,
                                 const MImServerXOptions &options)
    {
        if (not w) {
            return false;
        }

        w->setAttribute(Qt::WA_OpaquePaintEvent);
        w->setAttribute(Qt::WA_NoSystemBackground);
        w->setAutoFillBackground(false);
        // Be aware that one cannot verify whether the background role *is*
        // QPalette::NoRole - see QTBUG-17924.
        w->setBackgroundRole(QPalette::NoRole);

        if (!options.selfComposited) {
            // Careful: This flag can trigger a call to
            // qt_x11_recreateNativeWidgetsRecursive
            // - which will crash when it tries to get the effective WId
            // (as none of widgets have been mapped yet).
            w->setAttribute(Qt::WA_TranslucentBackground);
        }

        return true;
    }
}

MImXApplication::MImXApplication(int &argc,
                                 char**argv,
                                 const MImServerXOptions &options) :
    QApplication(argc, argv),
    mCompositeExtension(),
    mDamageExtension(),
    mBackgroundSuppressed(false),
    mPassThruWindow(),
    mRemoteWindow(),
    xOptions(options)
{
    mPassThruWindow.reset(new MPassThruWindow(this, xOptions));
    mPluginsProxyWidget.reset(new MImPluginsProxyWidget(xOptions, mPassThruWindow.get()));
    mRotationAnimation.reset(new MImRotationAnimation(pluginsProxyWidget(), passThruWindow(),
                                                      this, xOptions));

#ifdef HAVE_MEEGOGRAPHICSSYSTEM
    QMeeGoGraphicsSystemHelper::setSwitchPolicy(QMeeGoGraphicsSystemHelper::NoSwitch);
#endif

    configureWidgetsForCompositing();

    connect(this, SIGNAL(aboutToQuit()),
            this, SLOT(finalize()),
            Qt::UniqueConnection);
}

MImXApplication::~MImXApplication()
{
}

MImXApplication *MImXApplication::instance()
{
    MImXApplication *app = qobject_cast<MImXApplication *>(QCoreApplication::instance());
    if (QCoreApplication::instance() && !app) {
        qCritical() << "Application instance used is not MImXApplication";
    }
    return app;
}

void MImXApplication::finalize()
{
    // Cannot destroy QWidgets or QPixmaps during MIMApplication destruction.
    // Hence the finalize handler that runs before the d'tor.
    mPluginsProxyWidget.reset();
    mPassThruWindow.reset();
    mRemoteWindow.reset();
}

bool MImXApplication::x11EventFilter(XEvent *ev)
{
    handleTransientEvents(ev);
    handleRemoteWindowEvents(ev);
    handlePassThruMapEvent(ev);
    return QApplication::x11EventFilter(ev);
}

void MImXApplication::handleTransientEvents(XEvent *ev)
{
    if (not mRemoteWindow.get()) {
        return;
    }

    if (mRemoteWindow->wasIconified(ev) || mRemoteWindow->wasUnmapped(ev)) {
        qDebug() << "MImXApplication" << __PRETTY_FUNCTION__
                 << "Remote window was destroyed or iconified - hiding.";

        Q_EMIT remoteWindowChanged(0);
        Q_EMIT remoteWindowGone();
        mRemoteWindow.reset();
    }
}

void MImXApplication::handleRemoteWindowEvents(XEvent *event)
{
    if (not mRemoteWindow.get()) {
        return;
    }

    mRemoteWindow->handleEvent(event);
}

void MImXApplication::handlePassThruMapEvent(XEvent *ev)
{
    if (ev->type != MapNotify)
        return;

    if (ev->xmap.window != mPassThruWindow->effectiveWinId())
        return;

    if (not mRemoteWindow.get()) {
        qWarning() << __PRETTY_FUNCTION__
                   << "No remote window found, but passthru window was mapped.";
        return;
    }

    mRemoteWindow->resetPixmap();
}

void MImXApplication::setTransientHint(WId newRemoteWinId)
{
    if (0 == newRemoteWinId) {
        return;
    }

    if (mRemoteWindow.get() && mRemoteWindow->id() == newRemoteWinId) {
        return;
    }

    const bool wasRedirected(mRemoteWindow.get() && mRemoteWindow->isRedirected());

    mRemoteWindow.reset(new MImRemoteWindow(newRemoteWinId, this, xOptions));
    mRemoteWindow->setIMWidget(mPassThruWindow->window());

    connect(mRemoteWindow.get(), SIGNAL(contentUpdated(QRegion)),
            this,                SLOT(updatePassThruWindow(QRegion)));

    if (wasRedirected) {
        mRemoteWindow->redirect();
    }

    Q_EMIT remoteWindowChanged(mRemoteWindow.get());
}

void MImXApplication::setSuppressBackground(bool suppress)
{
    mBackgroundSuppressed = suppress;
}

QWidget *MImXApplication::passThruWindow() const
{
    return mPassThruWindow.get();
}

QWidget* MImXApplication::pluginsProxyWidget() const
{
    return mPluginsProxyWidget.get();
}

const QPixmap &MImXApplication::remoteWindowPixmap()
{
    if (not mRemoteWindow.get()
            || mBackgroundSuppressed
            || not xOptions.selfComposited) {
        static const QPixmap empty;
        return empty;
    }

    return mRemoteWindow->windowPixmap();
}

void MImXApplication::updatePassThruWindow(const QRegion &region)
{
    if (region.isEmpty() || remoteWindowPixmap().isNull()) {
        qDebug() << "Skipping update request for passthru window.";
        return;
    }

    mPassThruWindow->updateFromRemoteWindow(region);
}

void MImXApplication::visitWidgetHierarchy(WidgetVisitor visitor,
                                           QWidget *widget)
{
    std::deque<QWidget *> unvisited;
    unvisited.push_back(widget);

    // Breadth-first traversal of widget hierarchy, until no more
    // unvisited widgets remain. Will find viewports of QGraphicsViews,
    // as QAbstractScrollArea reparents the viewport to itself.
    while (not unvisited.empty()) {
        QWidget *current = unvisited.front();
        unvisited.pop_front();

        // If true, then continue walking the hiearchy of current widget.
        if (visitor(current, xOptions)) {
            // Mark children of current widget as unvisited:
            Q_FOREACH (QObject *obj, current->children()) {
                if (QWidget *w = qobject_cast<QWidget *>(obj)) {
                    unvisited.push_back(w);
                }
            }
        }
    }
}

void MImXApplication::configureWidgetsForCompositing()
{
    visitWidgetHierarchy(configureForCompositing, mPassThruWindow.get());
}

void MImXApplication::appOrientationAboutToChange(int toAngle) {
    mRotationAnimation->appOrientationAboutToChange(toAngle);
}

void MImXApplication::appOrientationChangeFinished(int toAngle) {
    mRotationAnimation->appOrientationChangeFinished(toAngle);
}

MImRemoteWindow *MImXApplication::remoteWindow() const
{
    return mRemoteWindow.get();
}
