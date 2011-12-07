#ifndef MALIIT_SERVER_INTERNAL_DEFAULTSURFACES_H
#define MALIIT_SERVER_INTERNAL_DEFAULTSURFACES_H

#include "surfaces.h"
#include "defaultsurface.h"

namespace Maliit {
namespace Server {
namespace Internal {

class DefaultSurfaces : public Maliit::Server::Internal::Surfaces
{
public:
    DefaultSurfaces();

    virtual std::tr1::shared_ptr<SurfaceFactory> factory();

    virtual void activate();
    virtual void deactivate();

    virtual void setRotation(Maliit::OrientationAngle angle);

    virtual bool supportsLegacyPlugins();
    virtual QWidget *legacySurface();

private:
    std::tr1::shared_ptr<DefaultSurfaceFactory> mSurfaceFactory;
};

class DefaultSurfacesFactory : public Maliit::Server::Internal::SurfacesFactory
{
public:
    virtual std::tr1::shared_ptr<Surfaces> createSurfaces();
};

} // namespace Internal
} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_INTERNAL_DEFAULTSURFACES_H
