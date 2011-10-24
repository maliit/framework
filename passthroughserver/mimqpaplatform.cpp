#include "mimqpaplatform.h"

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
        mProxyWidget.data()->hide();
    else
        mProxyWidget.data()->show();
}

QWidget *MImQPAPlatform::pluginsProxyWidget()
{
    return mProxyWidget.data();
}

