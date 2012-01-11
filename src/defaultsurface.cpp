#include "defaultsurface.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QWidget>

#if QT_VERSION >= 0x050000
#include <QGuiApplication>
#include <QPlatformNativeInterface>
#include <QVariant>
#include <QWindow>
#endif

namespace Maliit {
namespace Server {

class DefaultSurface : public virtual Surface
{
public:
    DefaultSurface(const SurfacePolicy &policy, QSharedPointer<DefaultSurface> parent, QWidget *toplevel)
        : Surface(),
          mPolicy(policy),
          mParent(parent),
          mToplevel(toplevel)
    {
        QWidget *parentWidget = 0;
        if (parent) {
            parentWidget = parent->mToplevel.data();
        }
        mToplevel->setParent(parentWidget, Qt::Window | Qt::FramelessWindowHint);
        mToplevel->setAttribute(Qt::WA_X11DoNotAcceptFocus);
        mToplevel->setAutoFillBackground(false);
        mToplevel->setBackgroundRole(QPalette::NoRole);

        mToplevel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        setSize(policy.defaultLandscapeSize());
//        mToplevel->hide();
    }

    ~DefaultSurface()
    {
    }

    void show()
    {
        mToplevel->show();
    }

    void hide()
    {
        mToplevel->hide();
    }

    QSize size()
    {
        return mToplevel->size();
    }

    void setSize(const QSize &size)
    {
        const QSize& desktopSize = QApplication::desktop()->size();

        qWarning() << desktopSize << QPoint(desktopSize.width() - size.width() / 2, desktopSize.height() - size.height());

        switch (mPolicy.position()) {
        case SurfacePositionCenterBottom:
            mToplevel->setGeometry(QRect(QPoint((desktopSize.width() - size.width()) / 2, desktopSize.height() - size.height()), size));
            break;
        default:
            mToplevel->setGeometry(QRect(QPoint(0, 0), size));
        }
    }

    QPoint relativePosition() const
    {
        return mToplevel->pos();
    }

    void setRelativePosition(const QPoint &position)
    {
        QPoint parentPosition;
        if (mParent) {
            parentPosition = mParent->relativePosition();
        }
        mToplevel->move(parentPosition + position);
    }


    QSharedPointer<Surface> parent()
    {
        return mParent;
    }

protected:
    SurfacePolicy mPolicy;
    QSharedPointer<DefaultSurface> mParent;
    QScopedPointer<QWidget> mToplevel;
};

class GraphicsView : public QGraphicsView
{
public:
    GraphicsView()
        : QGraphicsView()
    {
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_X11DoNotAcceptFocus);
        setAutoFillBackground(false);
        setBackgroundRole(QPalette::NoRole);

        setAttribute(Qt::WA_TranslucentBackground);
        viewport()->setAttribute(Qt::WA_TranslucentBackground);
    }

    bool event(QEvent *e)
    {
        if (e->type() == QEvent::WinIdChange) {
    #if QT_VERSION >= 0x050000
            QWindow *win = window()->windowHandle();
            if (win)
                QGuiApplication::platformNativeInterface()->setWindowProperty(win->handle(), "AcceptFocus", QVariant(false));
    #endif
        }
        return QWidget::event(e);
    }

};

class RootItem
    : public QGraphicsItem
{
private:
    QRectF m_rect;

public:
    explicit RootItem(QGraphicsItem *parent = 0)
        : QGraphicsItem(parent)
        , m_rect()
    {
        setFlag(QGraphicsItem::ItemHasNoContents);
    }

    void setRect(const QRectF &rect)
    {
        m_rect = rect;
    }

    virtual QRectF boundingRect() const
    {
        return m_rect;
    }

    virtual void paint(QPainter *,
                       const QStyleOptionGraphicsItem *,
                       QWidget *)
    {}
};

class DefaultGraphicsViewSurface : public DefaultSurface, public GraphicsViewSurface
{
public:
    DefaultGraphicsViewSurface(const SurfacePolicy &policy, QSharedPointer<DefaultSurface> parent)
        : DefaultSurface(policy, parent, new GraphicsView),
          GraphicsViewSurface(),
          mRoot(0)
    {
        view()->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
        view()->setOptimizationFlags(QGraphicsView::DontClipPainter | QGraphicsView::DontSavePainterState);
        view()->setFrameShape(QFrame::NoFrame);
        view()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        QGraphicsScene *scene = new QGraphicsScene(view());
        view()->setScene(scene);
        view()->setSceneRect(QRect(QPoint(0, 0), policy.defaultLandscapeSize()));
        view()->setSceneRect(QRect(QPoint(0, 0), policy.defaultLandscapeSize()));
    }

    ~DefaultGraphicsViewSurface() {}

    QGraphicsScene *scene()
    {
        return view()->scene();
    }

    QGraphicsView *view()
    {
        return static_cast<QGraphicsView*>(mToplevel.data());
    }

    void addItem(QGraphicsItem *item)
    {
        scene()->addItem(item);
    }

    void show()
    {
        DefaultSurface::show();

        const QRect &rect(view()->rect());

        if (not mRoot) {
            scene()->addItem(mRoot = new RootItem);
            mRoot->setRect(rect);
            mRoot->show();
        }
    }

    void clear()
    {
        mRoot = 0;
        scene()->clear();
    }

    QGraphicsItem *root()
    {
        return mRoot;
    }

private:
    RootItem *mRoot;
};

DefaultSurfaceFactory::DefaultSurfaceFactory()
    : surfaces()
{
}

DefaultSurfaceFactory::~DefaultSurfaceFactory()
{
}

QSharedPointer<GraphicsViewSurface> DefaultSurfaceFactory::createGraphicsViewSurface(const SurfacePolicy &policy, QSharedPointer<Surface> parent)
{
    QSharedPointer<DefaultSurface> defaultSurfaceParent(qSharedPointerDynamicCast<DefaultSurface>(parent));
    QSharedPointer<DefaultGraphicsViewSurface> newSurface(new DefaultGraphicsViewSurface(policy, defaultSurfaceParent));
    surfaces.push_back(newSurface);
    return newSurface;
}

} // namespace Server
} // namespace Maliit
