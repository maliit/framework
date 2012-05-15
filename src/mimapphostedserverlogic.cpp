#include "mimapphostedserverlogic.h"

#include "windowedsurfacegroup.h"
#include "maliit/plugins/abstractsurface.h" // for AbstractSurface::Option

#include <QtDebug>
#include <QGridLayout>

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
    QGridLayout *layout = new QGridLayout(mPluginsWidget);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(2, 1);

    mPluginsWidget->setAutoFillBackground(false);
    mPluginsWidget->setBackgroundRole(QPalette::NoRole);
    mPluginsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mPluginsWidget->setFocusPolicy(Qt::NoFocus);

    connect(mSurfaceGroupFactory.data(), SIGNAL(surfaceWidgetCreated(QWidget*,int)),
            this, SLOT(newSurfaceWidget(QWidget*,int)));
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

void MImAppHostedServerLogic::newSurfaceWidget(QWidget *widget, int surfaceOptions)
{
    visitWidgetHierarchy(configureForAppHosting, widget);

    if (surfaceOptions & Maliit::Plugins::AbstractSurface::PositionOverlay)
        return;

    QGridLayout *layout = qobject_cast<QGridLayout*>(mPluginsWidget->layout());

    if (surfaceOptions & Maliit::Plugins::AbstractSurface::PositionLeftBottom)
        layout->addWidget(widget, 0, 0, Qt::AlignCenter);
    else if (surfaceOptions & Maliit::Plugins::AbstractSurface::PositionCenterBottom)
        layout->addWidget(widget, 0, 1, Qt::AlignCenter);
    else if (surfaceOptions & Maliit::Plugins::AbstractSurface::PositionRightBottom)
        layout->addWidget(widget, 0, 2, Qt::AlignCenter);
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
