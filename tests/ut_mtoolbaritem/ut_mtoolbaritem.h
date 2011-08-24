#ifndef UT_MTOOLBARDATA_H
#define UT_MTOOLBARDATA_H

#include <QtTest/QtTest>
#include <QObject>

class QCoreApplication;
class MToolbarItem;

class Ut_MToolbarItem : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testSetProperty();
    void testHighlighted();
    void testCustom();
    void testEnabled();

private:
    QCoreApplication *app;
    MToolbarItem *subject;
};

#endif

