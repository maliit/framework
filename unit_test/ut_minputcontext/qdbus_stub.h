#ifndef QDBUS_STUB
#define QDBUS_STUB

#include <QDBusConnection>
#include <QDBusInterface>

/***************************************************************/
class QDBusInterfaceStub
{
public:
    virtual void QDBusInterfaceConstructor(const QString &, const QString &, const QString &, const QDBusConnection &, QObject *);
    virtual bool isValid() const;
    virtual QDBusMessage call(QDBus::CallMode mode, const QString &method, const QVariant &arg1 = QVariant(), const QVariant &arg2 = QVariant(), const QVariant &arg3 = QVariant(), const QVariant &arg4 = QVariant(), const QVariant &arg5 = QVariant(), const QVariant &arg6 = QVariant(), const QVariant &arg7 = QVariant(), const QVariant &arg8 = QVariant());

    QObject *target;
};

void
QDBusInterfaceStub::QDBusInterfaceConstructor(const QString &a, const QString &b, const QString &c, const QDBusConnection &conn, QObject *obj)
{
    Q_UNUSED(a);
    Q_UNUSED(b);
    Q_UNUSED(c);
    Q_UNUSED(conn);
    Q_UNUSED(obj);
}

bool
QDBusInterfaceStub::isValid() const
{
    return true;
}


QDBusMessage
QDBusInterfaceStub::call(QDBus::CallMode mode, const QString &method, const QVariant &arg1, const QVariant &arg2, const QVariant &arg3, const QVariant &arg4, const QVariant &arg5, const QVariant &arg6, const QVariant &arg7, const QVariant &arg8)
{
    Q_UNUSED(mode);
    QGenericArgument args[8];
    QList<QVariant> _args;

    _args << arg1 << arg2 << arg3 << arg4 << arg5 << arg6 << arg7 << arg8;

    for (int i = 0; i < 8; i ++) {
        void *data;
        switch (_args[i].type()) {
        case QVariant::Bool:
            data = new bool(_args[i].toBool());
            args[i] = Q_ARG(bool, *static_cast<bool *>(data));
            break;
        case QVariant::String:
            data = new QString(_args[i].toString());
            args[i] = Q_ARG(QString, *static_cast<QString *>(data));
            break;
        case QVariant::Int:
            data = new int(_args[i].toInt());
            args[i] = Q_ARG(int, *static_cast<int *>(data));
            break;

        case QVariant::Point:
            data = new QPoint(_args[i].toPoint());
            args[i] = Q_ARG(QPoint, *static_cast<QPoint *>(data));
            break;
        case QVariant::Rect:
            data = new QRect(_args[i].toRect());
            args[i] = Q_ARG(QRect, *static_cast<QRect *>(data));
            break;
        default:
            break;
        }
    }
    if (target) {
        qDebug() << method << ":" << QMetaObject::invokeMethod(target, method.toLatin1(), Qt::DirectConnection,
                 args[0], args[1], args[2], args[3],
                 args[4], args[5], args[6], args[7]);
    }
    QDBusMessage msg;
    return msg;
}

QDBusInterfaceStub defaultQDBusInterfaceStub;
QDBusInterfaceStub *qDBusInterfaceStub = &defaultQDBusInterfaceStub;

QDBusInterface::QDBusInterface(const QString &a, const QString &b, const QString &c, const QDBusConnection &conn, QObject *obj) :
    QDBusAbstractInterface(a, b, "stub", conn, obj)
{
    qDBusInterfaceStub->QDBusInterfaceConstructor(a, b, c, conn, obj);
}

bool
QDBusAbstractInterface::isValid() const
{
    return qDBusInterfaceStub->isValid();
}


QDBusMessage
QDBusAbstractInterface::call(QDBus::CallMode mode, const QString &method, const QVariant &arg1, const QVariant &arg2, const QVariant &arg3, const QVariant &arg4, const QVariant &arg5, const QVariant &arg6, const QVariant &arg7, const QVariant &arg8)
{
    return qDBusInterfaceStub->call(mode, method, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

/***************************************************************/
class QDBusConnectionStub : public QDBusConnection
{
public:
    QDBusConnectionStub();
    virtual QDBusConnection sessionBus();
    virtual bool registerObject(const QString &, QObject *, RegisterOptions options = ExportAdaptors);
    virtual bool registerService(const QString &);
    virtual bool isConnected() const;
    virtual QDBusConnectionInterface *interface() const;
};

QDBusConnectionStub::QDBusConnectionStub() : QDBusConnection("")
{
}

bool
QDBusConnectionStub::isConnected() const
{
    return true;
}

QDBusConnection
QDBusConnectionStub::sessionBus()
{
    QDBusConnectionStub connection;
    return connection;
}

bool
QDBusConnectionStub::registerObject(const QString &str, QObject *obj, RegisterOptions options)
{
    Q_UNUSED(str);
    Q_UNUSED(obj);
    Q_UNUSED(options);

    return true;
}


bool
QDBusConnectionStub::registerService(const QString &str)
{
    Q_UNUSED(str);
    return true;
}

QDBusConnectionInterface*
QDBusConnectionStub::interface() const
{
    return 0;
}

QDBusConnectionStub defaultConnectionStub;
QDBusConnectionStub *connectionStub = &defaultConnectionStub;


QDBusConnection QDBusConnection::sessionBus()
{
    return connectionStub->sessionBus();
}

bool QDBusConnection::isConnected() const
{
    return connectionStub->isConnected();
}

bool
QDBusConnection::registerObject(const QString &str, QObject *obj, RegisterOptions options)
{
    return connectionStub->registerObject(str, obj, options);
}

bool
QDBusConnection::registerService(const QString &str)
{
    return connectionStub->registerService(str);
}

QDBusConnectionInterface*
QDBusConnection::interface() const
{
    return connectionStub->interface();
}
#endif
