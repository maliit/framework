#include "mimqpaplatform.h"

#include "mimpluginsproxywidget.h"

#include <QDebug>
#include <QWidget>

MImQPAPlatform::MImQPAPlatform() :
    QObject(0),
    mProxyWidget(new MImPluginsProxyWidget)
{
}

void MImQPAPlatform::inputPassthrough(const QRegion &region)
{
    qDebug() << __PRETTY_FUNCTION__;

    if (region.isEmpty())
        mProxyWidget->hide();
    else
        mProxyWidget->show();
}

QWidget *MImQPAPlatform::pluginsProxyWidget()
{
    return mProxyWidget.get();
}

