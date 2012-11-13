
#include "quickviewsurfacefactory.h"

#include <maliit/plugins/abstractsurface.h>
#include <maliit/plugins/quickviewsurface.h>

#include <QtGui>
#include <Qt/QtQuick>

using Maliit::Plugins::AbstractSurface;

namespace Maliit {
namespace Server {

class QuickViewSurfaceImpl : public Maliit::Plugins::QuickViewSurface {
public:
    QuickViewSurfaceImpl(QuickViewSurfaceFactory *factory, AbstractSurface::Options options, const QSharedPointer<QuickViewSurfaceImpl> &parent)
        : QuickViewSurface(),
          mFactory(factory),
          mOptions(options),
          mParent(parent),
          mActive(false),
          mVisible(false),
          mRelativePosition(),
          mWindow()
    {
        QWindow *parentWindow = 0;
        if (parent) {
            parentWindow = parent->mWindow.data();
        }
        mWindow.reset(new QQuickView(parentWindow));

        mWindow->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint | Qt::WindowDoesNotAcceptFocus);

        QSurfaceFormat format;
        format.setAlphaBufferSize(8);
        mWindow->setFormat(format);
        mWindow->setColor(QColor(Qt::transparent));

        updateVisibility();
    }

    ~QuickViewSurfaceImpl()
    {
    }

    void show()
    {
        mVisible = true;
        updateVisibility();
    }

    void hide()
    {
        mVisible = false;
        updateVisibility();
    }

    QSize size() const
    {
        return mWindow->size();
    }

    void setSize(const QSize &size)
    {
        const QSize& desktopSize = QGuiApplication::screens().first()->size();

        // stand-alone Maliit server
         if (mOptions & PositionCenterBottom) {
             mWindow->setGeometry(QRect(QPoint((desktopSize.width() - size.width()) / 2, desktopSize.height() - size.height()), size));
         } else {
             mWindow->resize(size);
         }
    }

    QPoint relativePosition() const
    {
        return mRelativePosition;
    }

    void setRelativePosition(const QPoint &position)
    {
        mRelativePosition = position;
        QPoint parentPosition(0, 0);
        if (mParent) {
            parentPosition = mParent->mWindow->pos();
        }
        mWindow->setPos(parentPosition + mRelativePosition);
    }


    QSharedPointer<AbstractSurface> parent() const
    {
        return mParent;
    }

    QPoint translateEventPosition(const QPoint &eventPosition, const QSharedPointer<AbstractSurface> &eventSurface = QSharedPointer<AbstractSurface>()) const
    {
        if (!eventSurface)
            return eventPosition;

        QSharedPointer<QuickViewSurfaceImpl> windowedSurface = qSharedPointerDynamicCast<QuickViewSurfaceImpl>(eventSurface);
        if (!windowedSurface)
            return QPoint();

        return -mWindow->pos() + eventPosition + windowedSurface->mWindow->pos();
    }

    void setActive(bool active)
    {
        mActive = active;
        updateVisibility();
    }

    QRegion inputMethodArea()
    {
        if (!mWindow->isVisible())
            return QRegion();

        return QRegion(mWindow->geometry());
    }

    QQuickView *view() const
    {
        return mWindow.data();
    }

private:
    void updateVisibility()
    {
        mWindow->setVisible(mActive && mVisible);
    }

    QuickViewSurfaceFactory *mFactory;
    AbstractSurface::Options mOptions;
    QSharedPointer<QuickViewSurfaceImpl> mParent;
    bool mActive;
    bool mVisible;
    QPoint mRelativePosition;
    QScopedPointer<QQuickView> mWindow;
};

QuickViewSurfaceFactory::QuickViewSurfaceFactory()
{
}

QuickViewSurfaceFactory::~QuickViewSurfaceFactory()
{
}

QSize QuickViewSurfaceFactory::screenSize() const
{
    return QGuiApplication::screens().first()->size();
}

bool QuickViewSurfaceFactory::supported(Maliit::Plugins::AbstractSurface::Options options) const
{
    return options & Maliit::Plugins::AbstractSurface::TypeQuick2;
}

QSharedPointer<Maliit::Plugins::AbstractSurface> QuickViewSurfaceFactory::create(Maliit::Plugins::AbstractSurface::Options options, const QSharedPointer<Maliit::Plugins::AbstractSurface> &parent)
{
    QSharedPointer<QuickViewSurfaceImpl> defaultSurfaceParent(qSharedPointerDynamicCast<QuickViewSurfaceImpl>(parent));
    if (options & Maliit::Plugins::AbstractSurface::TypeQuick2) {
        QSharedPointer<QuickViewSurfaceImpl> newSurface(new QuickViewSurfaceImpl(this, options, defaultSurfaceParent));
        surfaces.push_back(newSurface);
        return newSurface;
    }
    return QSharedPointer<AbstractSurface>();
}

void QuickViewSurfaceFactory::activate()
{
    mActive = true;

    Q_FOREACH(QWeakPointer<QuickViewSurfaceImpl> weakSurface, surfaces) {
        QSharedPointer<QuickViewSurfaceImpl> surface = weakSurface.toStrongRef();
        if (surface)
            surface->setActive(true);
    }
}

void QuickViewSurfaceFactory::deactivate()
{
    mActive = false;

    Q_FOREACH(QWeakPointer<QuickViewSurfaceImpl> weakSurface, surfaces) {
        QSharedPointer<QuickViewSurfaceImpl> surface = weakSurface.toStrongRef();
        if (surface)
            surface->setActive(false);
    }
}

} // namespace Server
} // namespace Maliit
