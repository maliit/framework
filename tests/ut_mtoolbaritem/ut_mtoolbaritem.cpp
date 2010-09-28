#include "ut_mtoolbaritem.h"

#include <mtoolbaritem.h>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QSignalSpy>
#include <QVariant>
#include <QDebug>

namespace {
}

void Ut_MToolbarItem::initTestCase()
{
    char *argv[1] = { (char *) "ut_mimpluginloader" };
    int argc = 1;

    app = new QCoreApplication(argc, argv);

}

void Ut_MToolbarItem::cleanupTestCase()
{
    delete app;
}

void Ut_MToolbarItem::init()
{
    subject = new MToolbarItem("item", MInputMethod::ItemUndefined);
}

void Ut_MToolbarItem::cleanup()
{
    delete subject;
    subject = 0;
}

void Ut_MToolbarItem::testSetProperty()
{
    QSignalSpy spy(subject, SIGNAL(propertyChanged(const QString &)));

    QVERIFY(spy.isValid());

    subject->setProperty("text", QVariant(QString("some text")));
    QVERIFY(spy.count() == 1);
    QVERIFY(spy.first().count() == 1);
    QCOMPARE(spy.first().first().toString(), QString("text"));
    QCOMPARE(subject->text(), QString("some text"));
    spy.clear();

    subject->setProperty("text", QVariant(QString("some text")));
    QVERIFY(spy.count() == 0);

    subject->setProperty("textId", QVariant(QString("some text id")));
    QVERIFY(spy.count() == 1);
    QVERIFY(spy.first().count() == 1);
    QCOMPARE(spy.first().first().toString(), QString("textId"));
    spy.clear();

    subject->setProperty("textId", QVariant(QString("some text id")));
    QVERIFY(spy.count() == 0);

    subject->setProperty("icon", QVariant(QString("some icon")));
    QVERIFY(spy.count() == 1);
    QVERIFY(spy.first().count() == 1);
    QCOMPARE(spy.first().first().toString(), QString("icon"));
    spy.clear();

    subject->setProperty("icon", QVariant(QString("some icon")));
    QVERIFY(spy.count() == 0);

    subject->setProperty("visible", QVariant(false));
    QVERIFY(spy.count() == 1);
    QVERIFY(spy.first().count() == 1);
    QCOMPARE(spy.first().first().toString(), QString("visible"));
    spy.clear();

    subject->setProperty("visible", QVariant(false));
    QVERIFY(spy.count() == 0);

    subject->setProperty("toggle", QVariant(true));
    QVERIFY(spy.count() == 1);
    QVERIFY(spy.first().count() == 1);
    QCOMPARE(spy.first().first().toString(), QString("toggle"));
    spy.clear();

    subject->setProperty("toggle", QVariant(true));
    QVERIFY(spy.count() == 0);

    subject->setProperty("pressed", QVariant(true));
    QVERIFY(spy.count() == 1);
    QVERIFY(spy.first().count() == 1);
    QCOMPARE(spy.first().first().toString(), QString("pressed"));
    spy.clear();

    subject->setProperty("pressed", QVariant(true));
    QVERIFY(spy.count() == 0);
}

void Ut_MToolbarItem::testHighlighted()
{
    QSignalSpy spy(subject, SIGNAL(propertyChanged(const QString &)));

    QVERIFY(spy.isValid());

    QVERIFY(!subject->highlighted());

    subject->setHighlighted(false);
    QVERIFY(spy.isEmpty());

    subject->setHighlighted(true);
    QVERIFY(spy.count() == 1);
    QVERIFY(spy.first().count() == 1);
    QCOMPARE(spy.first().first().toString(), QString("highlighted"));
    spy.clear();

    subject->setHighlighted(true);
    QVERIFY(spy.isEmpty());
}

QTEST_APPLESS_MAIN(Ut_MToolbarItem)

