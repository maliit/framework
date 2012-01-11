#include "defaultsurfaces.h"

namespace Maliit {
namespace Server {
namespace Internal {

DefaultSurfaceGroup::DefaultSurfaceGroup()
    : SurfaceGroup(),
      mSurfaceFactory(new DefaultSurfaceFactory)
{
}

SurfaceFactory *DefaultSurfaceGroup::factory()
{
    return mSurfaceFactory.data();
}

void DefaultSurfaceGroup::activate()
{
}

void DefaultSurfaceGroup::deactivate()
{
}

void DefaultSurfaceGroup::setRotation(Maliit::OrientationAngle)
{
}

QSharedPointer<SurfaceGroup> DefaultSurfaceGroupFactory::createSurfaceGroup()
{
    return QSharedPointer<SurfaceGroup>(new DefaultSurfaceGroup);
}


} // namespace Internal
} // namespace Server
} // namespace Maliit
