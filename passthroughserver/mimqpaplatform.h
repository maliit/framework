#ifndef MIMQPAPLATFORM_H
#define MIMQPAPLATFORM_H

#include <QObject>
#include <QRegion>
#include <QWeakPointer>

class QWidget;

class MImQPAPlatform : public QObject
{
    Q_OBJECT
public:
    explicit MImQPAPlatform(QWidget *proxyWidget);

public Q_SLOTS:
    //! Set window ID for given region
    void inputPassthrough(const QRegion &region = QRegion());

private:
    QWeakPointer<QWidget> mProxyWidget;
};

#endif // MIMQPAPLATFORM_H
