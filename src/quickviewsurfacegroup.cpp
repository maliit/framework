#include "quickviewsurfacegroup.h"
#include "quickviewsurfacefactory.h"

#include <QtCore>
#include <QRegion>

namespace Maliit {
namespace Server {


QuickViewSurfaceGroup::QuickViewSurfaceGroup(QuickViewSurfaceFactory *surface_factory)
    : AbstractSurfaceGroup()
    , m_surface_factory(surface_factory)
{}


QuickViewSurfaceGroup::~QuickViewSurfaceGroup()
{}


Maliit::Plugins::AbstractSurfaceFactory * QuickViewSurfaceGroup::factory()
{
    return m_surface_factory.data();
}


void QuickViewSurfaceGroup::activate()
{
    m_surface_factory->activate();
}


void QuickViewSurfaceGroup::deactivate()
{
    m_surface_factory->deactivate();
}


void QuickViewSurfaceGroup::setRotation(Maliit::OrientationAngle angle)
{
    Q_UNUSED(angle)
}


QSharedPointer<AbstractSurfaceGroup> QuickViewSurfaceGroupFactory::createSurfaceGroup()
{
    QuickViewSurfaceFactory *factory = new QuickViewSurfaceFactory;

    QSharedPointer<QuickViewSurfaceGroup> group(new QuickViewSurfaceGroup(factory));
    m_groups.push_back(group);

    return group;
}

} // namespace Server
} // namespace Maliit
