#ifndef UT_MTOOLBARDATA_H
#define UT_MTOOLBARDATA_H

#include <QtTest/QtTest>
#include <QObject>

class QCoreApplication;
class MToolbarItem;

class Ut_MToolbarItem : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testSetProperty();
    void testHighlighted();
    void testCustom();
    void testEnabled();
    void testContentAlignment();

private:
    QCoreApplication *app;
    MToolbarItem *subject;
};

#endif

