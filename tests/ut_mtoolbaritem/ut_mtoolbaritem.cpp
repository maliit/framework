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
    spy.clear();

    subject->setProperty("enabled", QVariant(false));
    QVERIFY(spy.count() == 1);
    QVERIFY(spy.first().count() == 1);
    QCOMPARE(spy.first().first().toString(), QString("enabled"));
    spy.clear();

    subject->setProperty("enabled", QVariant(false));
    QVERIFY(spy.count() == 0);

    subject->setProperty("enabled", QVariant(true));
    QVERIFY(spy.count() == 1);
    QVERIFY(spy.first().count() == 1);
    QCOMPARE(spy.first().first().toString(), QString("enabled"));
    spy.clear();
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

void Ut_MToolbarItem::testCustom()
{
    QSignalSpy spy(subject, SIGNAL(propertyChanged(const QString &)));

    QVERIFY(spy.isValid());

    QVERIFY(subject->isCustom());

    subject->setCustom(true);
    QVERIFY(spy.isEmpty());

    subject->setCustom(false);
    QVERIFY(spy.count() == 1);
    QVERIFY(spy.first().count() == 1);
    QCOMPARE(spy.first().first().toString(), QString("custom"));
    spy.clear();

    subject->setCustom(false);
    QVERIFY(spy.isEmpty());
}

void Ut_MToolbarItem::testEnabled()
{
    QSignalSpy spy(subject, SIGNAL(propertyChanged(const QString &)));

    QVERIFY(spy.isValid());

    QVERIFY(subject->enabled());

    subject->setEnabled(true);
    QVERIFY(spy.isEmpty());

    subject->setEnabled(false);
    QVERIFY(!subject->enabled());
    QVERIFY(spy.count() == 1);
    QVERIFY(spy.first().count() == 1);
    QCOMPARE(spy.first().first().toString(), QString("enabled"));
    spy.clear();

    subject->setEnabled(false);
    QVERIFY(!subject->enabled());
    QVERIFY(spy.isEmpty());
}

void Ut_MToolbarItem::testContentAlignment()
{
    QSignalSpy spy(subject, SIGNAL(propertyChanged(const QString &)));

    QVERIFY(spy.isValid());

    QCOMPARE(subject->contentAlignment(), Qt::AlignCenter);

    subject->setContentAlignment(Qt::AlignCenter);
    QVERIFY(spy.isEmpty());

    subject->setContentAlignment(Qt::AlignLeft);
    QCOMPARE(subject->contentAlignment(), Qt::AlignLeft);
    QVERIFY(spy.count() == 1);
    QVERIFY(spy.first().count() == 1);
    QCOMPARE(spy.first().first().toString(), QString("contentAlignment"));
    spy.clear();

    subject->setContentAlignment(Qt::AlignLeft);
    QCOMPARE(subject->contentAlignment(), Qt::AlignLeft);
    QVERIFY(spy.isEmpty());
}

QTEST_APPLESS_MAIN(Ut_MToolbarItem)

