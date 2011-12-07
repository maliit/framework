#ifndef MALIIT_SERVER_DEFAULTSURFACE_H
#define MALIIT_SERVER_DEFAULTSURFACE_H

#include <vector>

#include "surface.h"

namespace Maliit {
namespace Server {

class DefaultSurface;

class DefaultSurfaceFactory : public SurfaceFactory
{
public:
    DefaultSurfaceFactory();
    virtual ~DefaultSurfaceFactory();

//    virtual std::tr1::shared_ptr<Surface> createSurface(const SurfacePolicy &policy);
    virtual std::tr1::shared_ptr<GraphicsViewSurface> createGraphicsViewSurface(const SurfacePolicy &policy, std::tr1::shared_ptr<Surface> parent = std::tr1::shared_ptr<Surface>());
//    std::tr1::shared_ptr<Surface> createSubSurface(const SurfacePolicy &policy, const std::tr1::shared_ptr<Surface> &parent, const QPoint &location);

    void activate();
    void deactivate();

private:
    std::vector<std::tr1::weak_ptr<DefaultSurface> > surfaces;
};


} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_DEFAULTSURFACE_H
