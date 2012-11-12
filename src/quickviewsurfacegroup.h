#ifndef MALIIT_SERVER_QUICKVIEWSURFACEGROUP_H
#define MALIIT_SERVER_QUICKVIEWSURFACEGROUP_H

#include "abstractsurfacegroupfactory.h"
#include "abstractsurfacegroup.h"

class QSize;

namespace Maliit {
namespace Server {
class QuickViewSurfaceFactory;


class QuickViewSurfaceGroup
    : public AbstractSurfaceGroup
{
private:
    const QScopedPointer<QuickViewSurfaceFactory> m_surface_factory;

public:
    explicit QuickViewSurfaceGroup(QuickViewSurfaceFactory *surface_factory);
    virtual ~QuickViewSurfaceGroup();

    virtual Maliit::Plugins::AbstractSurfaceFactory *factory();
    virtual void activate();
    virtual void deactivate();
    void setRotation(Maliit::OrientationAngle angle);
};

class QuickViewSurfaceGroupFactory
    : public AbstractSurfaceGroupFactory
{
private:
    QList<QWeakPointer<QuickViewSurfaceGroup> > m_groups;

public:
    virtual QSharedPointer<AbstractSurfaceGroup> createSurfaceGroup();
};

} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_QUICKVIEWSURFACEGROUP_H
