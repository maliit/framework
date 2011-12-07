#include "fullscreensurface.h"

namespace Maliit {
namespace Server {

FullscreenSurface::FullscreenSurface(WeakWidget proxyWidget)
    : Surface(),
      mCentralWidget(new QWidget(proxyWidget.data()))
{
}

void FullscreenSurface::setWidget(QWidget *)
{
}

void FullscreenSurface::setQuickSource(const QUrl &, QObject *)
{
}

void FullscreenSurface::show()
{
}

void FullscreenSurface::hide()
{
}

QSize FullscreenSurface::size()
{
    return QSize();
}

void FullscreenSurface::setSize(const QSize &)
{
}

QWidget *FullscreenSurface::centralWidget()
{
    return mCentralWidget.data();
}

FullscreenSurfaceFactory::FullscreenSurfaceFactory(WeakWidget proxyWidget)
    : SurfaceFactory(),
      mProxyWidget(proxyWidget),
      mSurface()
{
}

std::tr1::shared_ptr<Surface> FullscreenSurfaceFactory::createSurface(const SurfacePolicy &)
{
    return std::tr1::shared_ptr<Surface>();
}

} // namespace Server
} // namespace Maliit
