#include "mimapphostedserverlogic.h"

#include "windowedsurfacegroup.h"

#include <QtDebug>

#include <deque>

namespace
{
    bool configureForAppHosting(QWidget *w)
    {
        if (not w) {
            return false;
        }

        w->setFocusPolicy(Qt::NoFocus);
        return true;
    }
}

MImAppHostedServerLogic::MImAppHostedServerLogic(QObject *parent)
    : MImAbstractServerLogic(parent)
    , mPluginsWidget(new QWidget(&mDefaultParent))
    , mSurfaceGroupFactory(new Maliit::Server::WindowedSurfaceGroupFactory)
{
    mPluginsWidget->setAutoFillBackground(false);
    mPluginsWidget->setBackgroundRole(QPalette::NoRole);
    mPluginsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

MImAppHostedServerLogic::~MImAppHostedServerLogic()
{
}

void MImAppHostedServerLogic::inputPassthrough(const QRegion &region)
{
    if (region.isEmpty()) {
        mPluginsWidget->hide();
    } else {
        mPluginsWidget->show();
    }
}

QWidget *MImAppHostedServerLogic::pluginsProxyWidget() const
{
    return mPluginsWidget;
}

QSharedPointer<Maliit::Server::AbstractSurfaceGroupFactory> MImAppHostedServerLogic::surfaceGroupFactory() const
{
    return mSurfaceGroupFactory;
}

void MImAppHostedServerLogic::pluginLoaded()
{
    visitWidgetHierarchy(configureForAppHosting, mPluginsWidget);
}

void MImAppHostedServerLogic::appOrientationAboutToChange(int toAngle)
{
    Q_UNUSED(toAngle);
}

void MImAppHostedServerLogic::appOrientationChangeFinished(int toAngle)
{
    Q_UNUSED(toAngle);
}

void MImAppHostedServerLogic::applicationFocusChanged(WId newRemoteWinId)
{
    Q_UNUSED(newRemoteWinId);
}
