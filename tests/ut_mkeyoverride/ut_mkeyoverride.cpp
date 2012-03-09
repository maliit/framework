#include "ut_mkeyoverride.h"

#include <mkeyoverride.h>
#include <QFile>
#include <QDir>
#include <QSignalSpy>
#include <QVariant>
#include <QDebug>

namespace {
}
Q_DECLARE_METATYPE(MKeyOverride::KeyOverrideAttributes)

void Ut_MKeyOverride::initTestCase()
{
    qRegisterMetaType< MKeyOverride::KeyOverrideAttribute >("MKeyOverride::KeyOverrideAttribute");
    qRegisterMetaType< MKeyOverride::KeyOverrideAttributes >("MKeyOverride::KeyOverrideAttributes");
}

void Ut_MKeyOverride::cleanupTestCase()
{
}

void Ut_MKeyOverride::init()
{
    keyId = QString("testKey");
    subject = new MKeyOverride(keyId);
}

void Ut_MKeyOverride::cleanup()
{
    delete subject;
    subject = 0;
}

void Ut_MKeyOverride::testSetProperty()
{
    QSignalSpy spy(subject, SIGNAL(keyAttributesChanged(QString, MKeyOverride::KeyOverrideAttributes)));
    QVERIFY(spy.isValid());

    subject->setProperty("label", QVariant(QString("some text")));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.first().count(), 2);
    QCOMPARE(spy.first().first().toString(), keyId);
    QCOMPARE(spy.first().last().value<MKeyOverride::KeyOverrideAttributes>(),
             MKeyOverride::KeyOverrideAttributes(MKeyOverride::Label));
    QCOMPARE(subject->label(), QString("some text"));
    spy.clear();

    subject->setProperty("icon", QVariant(QString("some text")));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.first().count(), 2);
    QCOMPARE(spy.first().first().toString(), keyId);
    QCOMPARE(spy.first().last().value<MKeyOverride::KeyOverrideAttributes>(),
             MKeyOverride::KeyOverrideAttributes(MKeyOverride::Icon));
    QCOMPARE(subject->icon(), QString("some text"));
    spy.clear();

    subject->setProperty("highlighted", QVariant(true));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.first().count(), 2);
    QCOMPARE(spy.first().first().toString(), keyId);
    QCOMPARE(spy.first().last().value<MKeyOverride::KeyOverrideAttributes>(),
             MKeyOverride::KeyOverrideAttributes(MKeyOverride::Highlighted));
    QCOMPARE(subject->highlighted(), true);
    spy.clear();

    subject->setProperty("enabled", QVariant(false));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.first().count(), 2);
    QCOMPARE(spy.first().first().toString(), keyId);
    QCOMPARE(spy.first().last().value<MKeyOverride::KeyOverrideAttributes>(),
             MKeyOverride::KeyOverrideAttributes(MKeyOverride::Enabled));
    QCOMPARE(subject->enabled(), false);
    spy.clear();
}

QTEST_MAIN(Ut_MKeyOverride)

