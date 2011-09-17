#include "mimqpaplatform.h"

#include <QDebug>
#include <QWidget>

MImQPAPlatform::MImQPAPlatform(QWidget *proxyWidget) :
    QObject(0),
    mProxyWidget(proxyWidget)
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
