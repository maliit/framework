#include "mimapphostedserverlogic.h"

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
{
    QGridLayout *layout = new QGridLayout(mPluginsWidget);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(2, 1);

    mPluginsWidget->setAutoFillBackground(false);
    mPluginsWidget->setBackgroundRole(QPalette::NoRole);
    mPluginsWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mPluginsWidget->setFocusPolicy(Qt::NoFocus);
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
