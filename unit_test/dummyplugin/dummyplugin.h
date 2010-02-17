#ifndef DUMMYPLUGIN_H
#define DUMMYPLUGIN_H

#include <QObject>

class DummyPluginInterface
{
public:
    virtual QString name() = 0;
};

Q_DECLARE_INTERFACE(DummyPluginInterface,
                    "DummyPluginInterface/1.0")


//! Dummy plugin class for ut_duiimpluginloader
class DummyPlugin: public QObject,
    public DummyPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(DummyPluginInterface)

public:
    virtual QString name();
};

#endif
