#ifndef MIMXAPPLICATION_H
#define MIMXAPPLICATION_H

#include <QApplication>

#include "mimxserverlogic.h"

/*! \ingroup maliitserver
 * \brief Application abstraction for X applications.
 */
class MImXApplication : public QApplication
{
    Q_OBJECT
public:

    MImXApplication(int &argc, char** argv, const MImServerXOptions &options);
    virtual ~MImXApplication();
    static MImXApplication *instance();

    bool x11EventFilter(XEvent *ev);

    MImXServerLogic *serverLogic() const;

private:
    QScopedPointer<MImXServerLogic> mServerLogic;

    friend class Ut_PassthroughServer;
};

#endif // MIMXAPPLICATION_H
