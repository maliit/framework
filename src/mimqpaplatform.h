#ifndef MIMQPAPLATFORM_H
#define MIMQPAPLATFORM_H

#include <QObject>
#include <QRegion>
#include <QWeakPointer>

#include <memory>

class QWidget;

class MImQPAPlatform : public QObject
{
    Q_OBJECT
public:
    explicit MImQPAPlatform();

    QWidget *pluginsProxyWidget();

public Q_SLOTS:
    //! Set window ID for given region
    void inputPassthrough(const QRegion &region = QRegion());

private:
    std::auto_ptr<QWidget> mProxyWidget;
};

#endif // MIMQPAPLATFORM_H
