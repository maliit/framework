#ifndef MALIIT_SERVER_QUICKVIEWSURFACEFACTORY_P_H
#define MALIIT_SERVER_QUICKVIEWSURFACEFACTORY_P_H

#include "quickviewsurfacefactory.h"

#include <maliit/plugins/quickviewsurface.h>

#include <QtGui>
#include <QtQuick>

namespace Maliit {
namespace Server {

class QuickViewSurfaceImpl : public QObject,
                             public Maliit::Plugins::QuickViewSurface
{
    Q_OBJECT

public:
    QuickViewSurfaceImpl(QuickViewSurfaceFactory *factory, AbstractSurface::Options options,
                         const QSharedPointer<QuickViewSurfaceImpl> &parent);
    ~QuickViewSurfaceImpl();

    void show();
    void hide();

    QSize size() const;
    void setSize(const QSize &size);

    QPoint relativePosition() const;
    void setRelativePosition(const QPoint &position);

    QSharedPointer<AbstractSurface> parent() const;

    QPoint translateEventPosition(const QPoint &eventPosition,
                                  const QSharedPointer<AbstractSurface> &eventSurface = QSharedPointer<AbstractSurface>()) const;
    void setActive(bool active);

    QRegion inputMethodArea();

    QQuickView *view() const;

protected:
    bool eventFilter(QObject *, QEvent *event);

private:
    void updateVisibility();

    Q_SLOT void updatePosition();

    QuickViewSurfaceFactory *mFactory;
    AbstractSurface::Options mOptions;
    QSharedPointer<QuickViewSurfaceImpl> mParent;
    bool mActive;
    bool mVisible;
    QPoint mRelativePosition;
    QScopedPointer<QQuickView> mWindow;
};

} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_QUICKVIEWSURFACEFACTORY_H
