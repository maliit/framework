#ifndef UT_MTOOLBARDATA_H
#define UT_MTOOLBARDATA_H

#include <QtTest/QtTest>
#include <QObject>

class QCoreApplication;
class MToolbarData;

class Ut_MToolbarData : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testLoadXML();
    void testLandspaceOnly();

    void testLoadOldXML();
    void testMinimalXML();

    void testRefuseAttribute_data();
    void testRefuseAttribute();

    void testAddItem();

private:
    QCoreApplication *app;
    MToolbarData *subject;
};

#endif

