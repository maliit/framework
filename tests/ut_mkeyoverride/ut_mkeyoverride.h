#ifndef UT_MKEYOVERRIDE_H
#define UT_MKEYOVERRIDE_H

#include <QtTest/QtTest>
#include <QObject>

class QCoreApplication;
class MKeyOverride;

class Ut_MKeyOverride : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testSetProperty();

private:
    QCoreApplication *app;
    MKeyOverride *subject;
    QString keyId;
};

#endif

