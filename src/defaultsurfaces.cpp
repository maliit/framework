#include "defaultsurfaces.h"

namespace Maliit {
namespace Server {
namespace Internal {

DefaultSurfaces::DefaultSurfaces()
    : Surfaces(),
      mSurfaceFactory(new DefaultSurfaceFactory)
{
}

std::tr1::shared_ptr<SurfaceFactory> DefaultSurfaces::factory()
{
    return mSurfaceFactory;
}

void DefaultSurfaces::activate()
{
}

void DefaultSurfaces::deactivate()
{
}

void DefaultSurfaces::setRotation(Maliit::OrientationAngle)
{
}

bool DefaultSurfaces::supportsLegacyPlugins()
{
    return false;
}

QWidget *DefaultSurfaces::legacySurface()
{
    return 0;
}

std::tr1::shared_ptr<Surfaces> DefaultSurfacesFactory::createSurfaces()
{
    return std::tr1::shared_ptr<Surfaces>(new DefaultSurfaces);
}


} // namespace Internal
} // namespace Server
} // namespace Maliit
