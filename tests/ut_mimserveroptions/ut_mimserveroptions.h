#ifndef UT_MTOOLBARDATA_H
#define UT_MTOOLBARDATA_H

#include <mimserveroptions.h>

#include <QtTest/QtTest>
#include <QObject>

class QCoreApplication;

class Ut_MImServerOptions : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testCommonOptions_data();
    void testCommonOptions();

private:
    QCoreApplication *app;
    MImServerCommonOptions commonOptions;
    MImServerXOptions xOptions;
};

#endif

