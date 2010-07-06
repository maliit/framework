#ifndef FT_MIMSETTINGSAPPLET_H
#define FT_MIMSETTINGSAPPLET_H

#include <QtTest/QtTest>
#include <QObject>

class ft_MImSettingsApplet: public QObject
{
    Q_OBJECT

private slots:
    void testAppletIsLoadable();
};

#endif
