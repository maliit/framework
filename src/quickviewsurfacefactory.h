#ifndef MALIIT_SERVER_QUICKVIEWSURFACEFACTORY_H
#define MALIIT_SERVER_QUICKVIEWSURFACEFACTORY_H

#include <maliit/plugins/abstractsurfacefactory.h>

namespace Maliit {
namespace Server {

class QuickViewSurfaceImpl;

class QuickViewSurfaceFactory : public Maliit::Plugins::AbstractSurfaceFactory
{
public:
    QuickViewSurfaceFactory();
    virtual ~QuickViewSurfaceFactory();

    QSize screenSize() const;

    bool supported(Maliit::Plugins::AbstractSurface::Options options) const;
    QSharedPointer<Maliit::Plugins::AbstractSurface> create(Maliit::Plugins::AbstractSurface::Options options,
                                                            const QSharedPointer<Maliit::Plugins::AbstractSurface> &parent);

    void activate();
    void deactivate();

private:
    QList<QWeakPointer<QuickViewSurfaceImpl> > surfaces;
    bool mActive;
};

} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_QUICKVIEWSURFACEFACTORY_H
