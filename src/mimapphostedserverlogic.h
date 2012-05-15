#ifndef MIMAPPHOSTEDSERVERLOGIC_H
#define MIMAPPHOSTEDSERVERLOGIC_H

#include "mimabstractserverlogic.h"

#include <QWidget>

namespace Maliit {
namespace Server {
    class WindowedSurfaceGroupFactory;
}
}

class MImAppHostedServerLogic : public MImAbstractServerLogic
{
    Q_OBJECT
public:
    explicit MImAppHostedServerLogic(QObject *parent = 0);
    virtual ~MImAppHostedServerLogic();

    //! reimpl
    virtual QWidget *pluginsProxyWidget() const;
    virtual QSharedPointer<Maliit::Server::AbstractSurfaceGroupFactory> surfaceGroupFactory() const;
    //! reimpl_end

public Q_SLOTS:
    //! reimpl
    virtual void inputPassthrough(const QRegion &region = QRegion());
    virtual void appOrientationAboutToChange(int toAngle);
    virtual void appOrientationChangeFinished(int toAngle);
    virtual void applicationFocusChanged(WId remoteWinId);
    virtual void pluginLoaded();
    //! reimpl_end

private:
    Q_SLOT void newSurfaceWidget(QWidget *widget, int surfaceOptions);

    /* Used as the default parent widget for the plugins proxy widget
     * When the application reparents the plugins proxy widget into its widget hierachy
     * this relationship gets broken.
     * This lets destruction of the plugins proxy widget work in the same way for both cases. */
    QWidget mDefaultParent;
    QWidget *mPluginsWidget;
    QSharedPointer<Maliit::Server::WindowedSurfaceGroupFactory> mSurfaceGroupFactory;
};

#endif // MIMAPPHOSTEDSERVERLOGIC_H
