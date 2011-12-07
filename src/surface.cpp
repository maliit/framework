#include "surface.h"

#include <QApplication>
#include <QDesktopWidget>

namespace Maliit {
namespace Server {

SurfacePolicy::SurfacePolicy(const SurfacePosition position, const QSize &defaultLandscapeSize, const QSize &defaultPortraitSize)
    : mPosition(position),
      mDefaultLandscapeSize(defaultLandscapeSize),
      mDefaultPortraitSize(defaultPortraitSize)
{
}

SurfacePosition SurfacePolicy::position() const
{
    return mPosition;
}

QSize SurfacePolicy::defaultLandscapeSize() const
{
    return mDefaultLandscapeSize;
}

QSize SurfacePolicy::defaultPortraitSize() const
{
    return mDefaultPortraitSize;
}

Surface::~Surface()
{
}

SurfaceFactory::~SurfaceFactory()
{
}

QSize SurfaceFactory::screenLandscapeSize()
{
    return QApplication::desktop()->size();
}

QSize SurfaceFactory::screenPortraitSize()
{
    QSize size(QApplication::desktop()->size());

    size.transpose();

    return size;
}

} // namespace Server
} // namespace Maliit
