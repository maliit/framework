#ifndef MALIIT_SERVER_SURFACE_H
#define MALIIT_SERVER_SURFACE_H

#include <QPoint>
#include <QSharedPointer>
#include <QSize>
#include <QUrl>

#include <tr1/memory>
#include <tr1/functional>

class QGraphicsItem;
class QGraphicsScene;
class QGraphicsView;
class QWidget;

namespace Maliit {
namespace Server {

enum SurfacePosition {
    SurfacePositionInvalid,
    SurfacePositionOverlay,
    SurfacePositionCenterBottom,
    SurfacePositionLeftBottom,
    SurfacePositionRightBottom
};

class SurfacePolicy {
public:
    SurfacePolicy(const SurfacePosition position, const QSize &defaultLandscapeSize, const QSize &defaultPortraitSize);

    SurfacePosition position() const;
    QSize defaultLandscapeSize() const;
    QSize defaultPortraitSize() const;

private:
    const SurfacePosition mPosition;
    const QSize mDefaultLandscapeSize;
    const QSize mDefaultPortraitSize;
};

class Surface;

class Surface
{
public:
    virtual ~Surface();

    virtual void show() = 0;
    virtual void hide() = 0;

    virtual QSize size() = 0;
    virtual void setSize(const QSize &size) = 0;

    virtual QPoint relativePosition() const = 0;
    virtual void setRelativePosition(const QPoint &position) = 0;

    virtual QSharedPointer<Surface> parent() = 0;
};

class GraphicsViewSurface : public virtual Surface
{
public:
    virtual QGraphicsScene *scene() = 0;
    virtual QGraphicsView *view() = 0;

    virtual void addItem(QGraphicsItem *item) = 0;

    virtual QGraphicsItem *root() = 0;
    virtual void clear() = 0;
};


class SurfaceFactory
{
public:
    virtual ~SurfaceFactory();

    virtual QSize screenLandscapeSize();
    virtual QSize screenPortraitSize();

    virtual QSharedPointer<GraphicsViewSurface> createGraphicsViewSurface(const SurfacePolicy &policy, QSharedPointer<Surface> parent = QSharedPointer<Surface>()) = 0;

    void setActive();
};

} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_SURFACE_H
