#ifndef MALIIT_SERVER_INTERNAL_SURFACES_H
#define MALIIT_SERVER_INTERNAL_SURFACES_H

#include "surface.h"
#include "maliit/namespace.h"

namespace Maliit {
namespace Server {
namespace Internal {

class Surfaces
{
public:
    virtual std::tr1::shared_ptr<SurfaceFactory> factory() = 0;

    virtual void activate() = 0;
    virtual void deactivate() = 0;

    virtual void setRotation(Maliit::OrientationAngle angle) = 0;

    virtual bool supportsLegacyPlugins() = 0;
    virtual QWidget *legacySurface() = 0;
};

class SurfacesFactory
{
public:
    virtual ~SurfacesFactory();

    virtual std::tr1::shared_ptr<Surfaces> createSurfaces() = 0;
};

}
}
}

#endif // MALIIT_SERVER_INTERNAL_SURFACES_H
