#ifndef MALIIT_SERVER_DEFAULTSURFACE_H
#define MALIIT_SERVER_DEFAULTSURFACE_H

#include <QWeakPointer>

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

    virtual QSharedPointer<GraphicsViewSurface> createGraphicsViewSurface(const SurfacePolicy &policy, QSharedPointer<Surface> parent = QSharedPointer<Surface>());

    void activate();
    void deactivate();

private:
    std::vector<QWeakPointer<DefaultSurface> > surfaces;
};


} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_DEFAULTSURFACE_H
