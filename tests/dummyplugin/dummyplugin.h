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


//! Dummy plugin class for ut_mimpluginloader
class DummyPlugin: public QObject,
    public DummyPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(DummyPluginInterface)
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_PLUGIN_METADATA(IID  "org.maliit.tests.dummyplugin"
                      FILE "dummyplugin.json")
#endif

public:
    virtual QString name();
};

#endif
