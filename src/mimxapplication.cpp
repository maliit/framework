#include "mimxapplication.h"
#include "mimxserverlogic.h"

#include <QDebug>

MImXApplication::MImXApplication(int &argc,
                                 char**argv,
                                 const MImServerXOptions &options) :
    QApplication(argc, argv),
    mServerLogic(new MImXServerLogic(options))
{
    connect(this, SIGNAL(aboutToQuit()),
            mServerLogic.data(), SLOT(finalize()),
            Qt::UniqueConnection);
}

MImXApplication::~MImXApplication()
{
}

MImXApplication *MImXApplication::instance()
{
    MImXApplication *app = qobject_cast<MImXApplication *>(QCoreApplication::instance());
    if (QCoreApplication::instance() && !app) {
        qCritical() << "Application instance used is not MImXApplication";
    }
    return app;
}

bool MImXApplication::x11EventFilter(XEvent *ev)
{
    mServerLogic->x11EventFilter(ev);
    return QApplication::x11EventFilter(ev);
}

MImXServerLogic *MImXApplication::serverLogic() const
 {
    return mServerLogic.data();
 }
