#ifndef MALIIT_SERVER_FULLSCREENSURFACE_H
#define MALIIT_SERVER_FULLSCREENSURFACE_H

#include "surface.h"

#include <QWeakPointer>
#include <QWidget>

namespace Maliit {
namespace Server {

typedef QWeakPointer<QWidget> WeakWidget;

class FullscreenSurface : public Surface
{
public:
    FullscreenSurface(WeakWidget proxyWidget);

    void setWidget(QWidget *widget);
    void setQuickSource(const QUrl &source, QObject *object);

    void show();
    void hide();

    QSize size();
    void setSize(const QSize &size);

    QWidget *centralWidget();

private:
    WeakWidget mCentralWidget;
};

class FullscreenSurfaceFactory : public SurfaceFactory
{
public:
    FullscreenSurfaceFactory(WeakWidget proxyWidget);

    std::tr1::shared_ptr<Surface> createSurface(const SurfacePolicy &policy);

private:
    WeakWidget mProxyWidget;
    std::tr1::shared_ptr<Surface> mSurface;
};

} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_LEGACYSURFACE_H
