#ifndef MALIIT_PLUGINS_QUICKVIEWSURFACE_H
#define MALIIT_PLUGINS_QUICKVIEWSURFACE_H

#include <maliit/plugins/abstractsurface.h>

class QQuickView;

namespace Maliit {
namespace Plugins {

class QuickViewSurface : public AbstractSurface
{
public:
    virtual ~QuickViewSurface();

    virtual QQuickView *view() const = 0;
};

} // namespace Plugins
} // namespace Maliit

#endif // MALIIT_PLUGINS_QUICKVIEWSURFACE_H
