#ifndef UT_MKEYOVERRIDE_H
#define UT_MKEYOVERRIDE_H

#include <QtTest/QtTest>
#include <QObject>

class MKeyOverride;

class Ut_MKeyOverride : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testSetProperty();

private:
    MKeyOverride *subject;
    QString keyId;
};

#endif

