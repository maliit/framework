#ifndef MALIIT_SERVER_INTERNAL_DEFAULTSURFACES_H
#define MALIIT_SERVER_INTERNAL_DEFAULTSURFACES_H

#include "surfaces.h"
#include "defaultsurface.h"

namespace Maliit {
namespace Server {
namespace Internal {

class DefaultSurfaceGroup : public Maliit::Server::Internal::SurfaceGroup
{
public:
    DefaultSurfaceGroup();

    virtual SurfaceFactory *factory();

    virtual void activate();
    virtual void deactivate();

    virtual void setRotation(Maliit::OrientationAngle angle);

private:
    QScopedPointer<DefaultSurfaceFactory> mSurfaceFactory;
};

class DefaultSurfaceGroupFactory : public Maliit::Server::Internal::SurfaceGroupFactory
{
public:
    virtual QSharedPointer<SurfaceGroup> createSurfaceGroup();
};

} // namespace Internal
} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_INTERNAL_DEFAULTSURFACES_H
