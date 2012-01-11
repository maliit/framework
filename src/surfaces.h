#ifndef MALIIT_SERVER_INTERNAL_SURFACES_H
#define MALIIT_SERVER_INTERNAL_SURFACES_H

#include "surface.h"
#include "maliit/namespace.h"

namespace Maliit {
namespace Server {
namespace Internal {

class SurfaceGroup
{
public:
    virtual SurfaceFactory *factory() = 0;

    virtual void activate() = 0;
    virtual void deactivate() = 0;

    virtual void setRotation(Maliit::OrientationAngle angle) = 0;
};

class SurfaceGroupFactory
{
public:
    virtual ~SurfaceGroupFactory();

    virtual QSharedPointer<SurfaceGroup> createSurfaceGroup() = 0;
};

}
}
}

#endif // MALIIT_SERVER_INTERNAL_SURFACES_H
