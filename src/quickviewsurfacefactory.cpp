
#include "quickviewsurfacefactory.h"
#include "quickviewsurfacefactory_p.h"

#include <maliit/plugins/abstractsurface.h>
#include <maliit/plugins/quickviewsurface.h>

#include <QtGui>
#include <QtQuick>

using Maliit::Plugins::AbstractSurface;

namespace Maliit {
namespace Server {

QuickViewSurfaceImpl::QuickViewSurfaceImpl(QuickViewSurfaceFactory *factory, AbstractSurface::Options options,
                                           const QSharedPointer<QuickViewSurfaceImpl> &parent)
    : QuickViewSurface(),
      mFactory(factory),
      mOptions(options),
      mParent(parent),
      mActive(false),
      mVisible(false),
      mRelativePosition(),
      mWindow()
{
    mWindow.reset(new QQuickView(0));
    if (parent) {
        QWindow *parentWindow = parent->mWindow.data();
        mWindow->setTransientParent(parentWindow);
        parentWindow->installEventFilter(this);
        connect(parentWindow, SIGNAL(xChanged(int)), this, SLOT(updatePosition()));
        connect(parentWindow, SIGNAL(yChanged(int)), this, SLOT(updatePosition()));
    }

    mWindow->setFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint
                      | Qt::X11BypassWindowManagerHint | Qt::WindowDoesNotAcceptFocus);

    QSurfaceFormat format;
    format.setAlphaBufferSize(8);
    mWindow->setFormat(format);
    mWindow->setColor(QColor(Qt::transparent));

    updateVisibility();
}

QuickViewSurfaceImpl::~QuickViewSurfaceImpl()
{
}

void QuickViewSurfaceImpl::show()
{
    mVisible = true;
    updateVisibility();
}

void QuickViewSurfaceImpl::hide()
{
    mVisible = false;
    updateVisibility();
}

QSize QuickViewSurfaceImpl::size() const
{
    return mWindow->size();
}

void QuickViewSurfaceImpl::setSize(const QSize &size)
{
    const QSize& desktopSize = QGuiApplication::screens().first()->size();

    // stand-alone Maliit server
    if (mOptions & PositionCenterBottom) {
        mWindow->setGeometry(QRect(QPoint((desktopSize.width() - size.width()) / 2,
                                          desktopSize.height() - size.height()), size));
    } else {
        mWindow->resize(size);
    }
}

QPoint QuickViewSurfaceImpl::relativePosition() const
{
    return mRelativePosition;
}

void QuickViewSurfaceImpl::setRelativePosition(const QPoint &position)
{
    mRelativePosition = position;
    QPoint parentPosition(0, 0);
    if (mParent) {
        parentPosition = mParent->mWindow->position();
    }
    mWindow->setPosition(parentPosition + mRelativePosition);
}

QSharedPointer<AbstractSurface> QuickViewSurfaceImpl::parent() const
{
    return mParent;
}

QPoint QuickViewSurfaceImpl::translateEventPosition(const QPoint &eventPosition,
                                                    const QSharedPointer<AbstractSurface> &eventSurface) const
{
    if (!eventSurface)
        return eventPosition;

    QSharedPointer<QuickViewSurfaceImpl> windowedSurface = qSharedPointerDynamicCast<QuickViewSurfaceImpl>(eventSurface);
    if (!windowedSurface)
        return QPoint();

    return -mWindow->position() + eventPosition + windowedSurface->mWindow->position();
}

void QuickViewSurfaceImpl::setActive(bool active)
{
    mActive = active;
    updateVisibility();
}

QRegion QuickViewSurfaceImpl::inputMethodArea()
{
    if (!mWindow->isVisible())
        return QRegion();

    return QRegion(mWindow->geometry());
}

QQuickView *QuickViewSurfaceImpl::view() const
{
    return mWindow.data();
}

bool QuickViewSurfaceImpl::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::Move) {
        updatePosition();
    }

    return false;
}

void QuickViewSurfaceImpl::updateVisibility()
{
    mWindow->setVisible(mActive && mVisible);
}

void QuickViewSurfaceImpl::updatePosition()
{
    setRelativePosition(mRelativePosition);
}

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

QSharedPointer<Maliit::Plugins::AbstractSurface> QuickViewSurfaceFactory::create(Maliit::Plugins::AbstractSurface::Options options,
                                                                                 const QSharedPointer<Maliit::Plugins::AbstractSurface> &parent)
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
