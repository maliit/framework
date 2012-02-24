#include "ut_mtoolbaritem.h"

#include <mtoolbaritem.h>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QSignalSpy>
#include <QVariant>
#include <QDebug>

namespace {
    // we need for this shortcut due to problems with Qt metatypes
    typedef MToolbarItem::PropertyId PropertyId;

    QVariant readProperty(MToolbarItem * item, PropertyId id)
    {
        switch(id) {
        case MToolbarItem::Visible:
            return item->isVisible();
        case MToolbarItem::Text:
            return item->text();
        case MToolbarItem::TextId:
            return item->textId();
        case MToolbarItem::Toggle:
            return item->toggle();
        case MToolbarItem::Pressed:
            return item->pressed();
        case MToolbarItem::Highlighted:
            return item->highlighted();
        case MToolbarItem::Enabled:
            return item->enabled();
        case MToolbarItem::Icon:
            return item->icon();
        case MToolbarItem::IconId:
            return item->iconId();
        case MToolbarItem::Size:
            return item->size();
        case MToolbarItem::ContentAlignment:
            return QVariant(item->contentAlignment());
        case MToolbarItem::Nothing:
            break;
        }

        return QVariant();
    }
}

void Ut_MToolbarItem::initTestCase()
{
    char *argv[1] = { (char *) "ut_mimpluginloader" };
    int argc = 1;

    app = new QCoreApplication(argc, argv);

    qRegisterMetaType<PropertyId>("PropertyId");
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

void Ut_MToolbarItem::testSetProperty_data()
{
    QTest::addColumn<QString>("propertyName");
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<PropertyId>("expectedId");

    QTest::newRow("text")    << "text"    << QVariant(QString("some text"))    << MToolbarItem::Text;
    QTest::newRow("textId")  << "textId"  << QVariant(QString("some text id")) << MToolbarItem::TextId;
    QTest::newRow("icon")    << "icon"    << QVariant(QString("some icon"))    << MToolbarItem::Icon;
    QTest::newRow("iconId")  << "iconId"  << QVariant(QString("some icon id")) << MToolbarItem::IconId;
    QTest::newRow("visible") << "visible" << QVariant(false)                   << MToolbarItem::Visible;
    QTest::newRow("toggle")  << "toggle"  << QVariant(true)                    << MToolbarItem::Toggle;
    QTest::newRow("pressed") << "pressed" << QVariant(true)                    << MToolbarItem::Pressed;
    QTest::newRow("visible") << "visible" << QVariant(false)                   << MToolbarItem::Visible;
    QTest::newRow("enabled") << "enabled" << QVariant(false)                   << MToolbarItem::Enabled;
    QTest::newRow("size")    << "size"    << QVariant(50)                      << MToolbarItem::Size;

    QTest::newRow("highlighted") << "highlighted" << QVariant(true) << MToolbarItem::Highlighted;

    QTest::newRow("contentAlignment") << "contentAlignment" << QVariant(Qt::AlignLeft)
                                      << MToolbarItem::ContentAlignment;

}

void Ut_MToolbarItem::testSetProperty()
{
    QFETCH(QString, propertyName);
    QFETCH(QVariant, value);
    QFETCH(PropertyId, expectedId);

    QSignalSpy spy(subject, SIGNAL(propertyChanged(PropertyId)));
    QSignalSpy spyString(subject, SIGNAL(propertyChanged(QString)));

    QVERIFY(spy.isValid());
    QVERIFY(spyString.isValid());

    subject->setProperty(propertyName.toLatin1().data(), value);
    QVERIFY(spy.count() == 1);
    QVERIFY(spy.first().count() == 1);
    QCOMPARE(spy.first().first().value<MToolbarItem::PropertyId>(), expectedId);
    QCOMPARE(readProperty(subject, expectedId), value);
    spy.clear();
    QVERIFY(spyString.count() == 1);
    QVERIFY(spyString.first().count() == 1);
    QCOMPARE(spyString.first().first().toString(), propertyName);
    spyString.clear();

    subject->setProperty(propertyName.toLatin1().data(), value);
    QVERIFY(spy.count() == 0);
    QVERIFY(spyString.count() == 0);

    QCOMPARE(propertyName, QString(MToolbarItem::propertyName(expectedId)));
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

QTEST_APPLESS_MAIN(Ut_MToolbarItem)

