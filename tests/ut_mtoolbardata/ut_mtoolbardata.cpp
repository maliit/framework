#include "ut_mtoolbardata.h"

#include <mtoolbardata.h>
#include <mtoolbarlayout.h>
#include <mtoolbaritem.h>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QDebug>

namespace {
    QString Toolbar1 = "/toolbar1.xml";
    QString Toolbar2 = "/toolbar2.xml";
    QString Toolbar3 = "/toolbar3.xml";
    QString Toolbar4 = "/toolbar4.xml";
}

void Ut_MToolbarData::initTestCase()
{
    static char *argv[1] = { (char *) "ut_mimpluginloader" };
    static int argc = 1;

    app = new QCoreApplication(argc, argv);

    Toolbar1 = QCoreApplication::applicationDirPath() + Toolbar1;
    QVERIFY2(QFile(Toolbar1).exists(), "toolbar1.xml does not exist");
    Toolbar2 = QCoreApplication::applicationDirPath() + Toolbar2;
    QVERIFY2(QFile(Toolbar2).exists(), "toolbar2.xml does not exist");
    Toolbar3 = QCoreApplication::applicationDirPath() + Toolbar3;
    QVERIFY2(QFile(Toolbar3).exists(), "toolbar3.xml does not exist");
    Toolbar4 = QCoreApplication::applicationDirPath() + Toolbar4;
    QVERIFY2(QFile(Toolbar4).exists(), "toolbar4.xml does not exist");
}

void Ut_MToolbarData::cleanupTestCase()
{
    delete app;
}

void Ut_MToolbarData::init()
{
    subject = new MToolbarData;
}

void Ut_MToolbarData::cleanup()
{
    delete subject;
    subject = 0;
}

void Ut_MToolbarData::testLoadXML()
{
    qDebug() << "Try to load file" << Toolbar1;
    QVERIFY2(QFile(Toolbar1).exists(), "toolbar1.xml does not exist");
    bool ok;
    QSharedPointer<MToolbarItem> item;

    ok = subject->loadToolbarXml(Toolbar1);
    QVERIFY2(ok, "toolbar1.xml was not loaded correctly");

    QVERIFY(subject->isVisible() == true);

    //test landscape part
    QSharedPointer<const MToolbarLayout> landscape = subject->layout(MInputMethod::Landscape);
    QVERIFY(!landscape.isNull());
    QCOMPARE(landscape->items().count(), 4);

    item = landscape->items().at(0);
    QVERIFY(!item.isNull());
    QCOMPARE(item->type(), MInputMethod::ItemButton);
    QCOMPARE(item->name(), QString("test1"));
    QCOMPARE(item->group(), QString("group1"));
    QCOMPARE(int(item->showOn()), int(MInputMethod::VisibleAlways));
    QCOMPARE(int(item->hideOn()), int(MInputMethod::VisibleUndefined));
    QCOMPARE(item->alignment(), Qt::AlignRight);
    QCOMPARE(item->text(), QString("text1"));
    QCOMPARE(item->textId(), QString("id1"));
    QCOMPARE(item->isVisible(), true);
    QCOMPARE(item->toggle(), false);
    QCOMPARE(item->pressed(), false);
    QCOMPARE(item->icon(), QString("icon1"));
    QCOMPARE(item->size(), 100);
    QVERIFY(item->highlighted());
    QCOMPARE(item->enabled(), true);

    QCOMPARE(item->actions().count(), 2);
    QCOMPARE(item->actions().at(0)->type(), MInputMethod::ActionSendString);
    QCOMPARE(item->actions().at(0)->keys(), QString());
    QCOMPARE(item->actions().at(0)->text(), QString("sendstring1"));
    QCOMPARE(item->actions().at(0)->command(), QString());
    QCOMPARE(item->actions().at(0)->group(), QString());
    QCOMPARE(item->actions().at(1)->group(), QString("group3"));

    item = landscape->items().at(1);
    QVERIFY(!item.isNull());
    QCOMPARE(item->type(), MInputMethod::ItemButton);
    QCOMPARE(item->name(), QString("test2"));
    QCOMPARE(item->group(), QString("group2"));
    QCOMPARE(int(item->showOn()), int(MInputMethod::VisibleWhenSelectingText));
    QCOMPARE(int(item->hideOn()), int(MInputMethod::VisibleUndefined));
    QCOMPARE(item->alignment(), Qt::AlignCenter);
    QCOMPARE(item->text(), QString("text2"));
    QCOMPARE(item->textId(), QString("id2"));
    QCOMPARE(item->isVisible(), true);
    QCOMPARE(item->toggle(), true);
    QCOMPARE(item->pressed(), false);
    QCOMPARE(item->icon(), QString("icon2"));
    QCOMPARE(item->size(), 50);
    QVERIFY(!item->highlighted());
    QCOMPARE(item->enabled(), true);

    QCOMPARE(item->actions().count(), 3);
    QCOMPARE(item->actions().at(0)->type(), MInputMethod::ActionCopy);
    QCOMPARE(item->actions().at(0)->keys(), QString());
    QCOMPARE(item->actions().at(0)->text(), QString());
    QCOMPARE(item->actions().at(0)->command(), QString());
    QCOMPARE(item->actions().at(0)->group(), QString());
    QCOMPARE(item->actions().at(1)->group(), QString("group3"));
    QCOMPARE(item->actions().at(2)->keys(), QString("Ctrl+C"));

    item = landscape->items().at(2);
    QVERIFY(!item.isNull());
    QCOMPARE(item->type(), MInputMethod::ItemButton);
    QCOMPARE(item->name(), QString("test3"));
    QCOMPARE(item->group(), QString("group3"));
    QCOMPARE(int(item->showOn()), int(MInputMethod::VisibleAlways));
    QCOMPARE(int(item->hideOn()), int(MInputMethod::VisibleUndefined));
    QCOMPARE(item->alignment(), Qt::AlignLeft);
    QCOMPARE(item->text(), QString("text3"));
    QCOMPARE(item->textId(), QString("id3"));
    QCOMPARE(item->isVisible(), true);
    QCOMPARE(item->toggle(), false);
    QCOMPARE(item->pressed(), false);
    QCOMPARE(item->icon(), QString(""));
    QCOMPARE(item->iconId(), QString("icon3"));
    QCOMPARE(item->enabled(), false);

    QCOMPARE(item->actions().count(), 1);
    QCOMPARE(int(item->actions().at(0)->type()), int(MInputMethod::ActionPaste));
    QCOMPARE(item->actions().at(0)->keys(), QString());
    QCOMPARE(item->actions().at(0)->text(), QString());
    QCOMPARE(item->actions().at(0)->command(), QString());
    QCOMPARE(item->actions().at(0)->group(), QString());

    // test portrait part
    QSharedPointer<const MToolbarLayout> portrait = subject->layout(MInputMethod::Portrait);
    QVERIFY(!portrait.isNull());
    QCOMPARE(portrait->items().count(), 4);

    //items are shared between layouts
    QVERIFY(landscape->items().at(1) == portrait->items().at(0));
    QVERIFY(landscape->items().at(2) == portrait->items().at(1));
    QVERIFY(landscape->items().at(3) == portrait->items().at(2));

    item = portrait->items().at(3);
    QVERIFY(!item.isNull());
    QCOMPARE(item->type(), MInputMethod::ItemButton);
    QCOMPARE(item->name(), QString("test5"));
}

void Ut_MToolbarData::testLandspaceOnly()
{
    qDebug() << "Try to load file" << Toolbar2;
    QVERIFY2(QFile(Toolbar2).exists(), "toolbar2.xml does not exist");
    bool ok;

    ok = subject->loadToolbarXml(Toolbar2);
    QVERIFY2(ok, "toolbar2.xml was not loaded correctly");

    QVERIFY(subject->isVisible() == false);

    //test landscape part
    QSharedPointer<const MToolbarLayout> landscape = subject->layout(MInputMethod::Landscape);
    QVERIFY(!landscape.isNull());
    QCOMPARE(landscape->items().count(), 5);

    //verify at least some item's attributes
    QCOMPARE(landscape->items().at(0)->type(), MInputMethod::ItemButton);
    QCOMPARE(landscape->items().at(1)->type(), MInputMethod::ItemButton);
    QCOMPARE(landscape->items().at(2)->type(), MInputMethod::ItemButton);
    QCOMPARE(landscape->items().at(3)->type(), MInputMethod::ItemLabel);
    QCOMPARE(landscape->items().at(4)->type(), MInputMethod::ItemButton);

    QVERIFY(subject->layout(MInputMethod::Landscape) == subject->layout(MInputMethod::Portrait));
}

void Ut_MToolbarData::testLoadOldXML()
{
    QVERIFY2(QFile(Toolbar3).exists(), "toolbar3.xml does not exist");
    bool ok;

    ok = subject->loadToolbarXml(Toolbar3);
    QVERIFY2(ok, "toolbar3.xml was not loaded correctly");

    QVERIFY(subject->isVisible() == true);

    //test landscape part
    QSharedPointer<const MToolbarLayout> landscape = subject->layout(MInputMethod::Landscape);
    QVERIFY(!landscape.isNull());
    QCOMPARE(landscape->items().count(), 5);

    QVERIFY(subject->layout(MInputMethod::Landscape) == subject->layout(MInputMethod::Portrait));
}

void Ut_MToolbarData::testMinimalXML()
{
    bool ok;

    ok = subject->loadToolbarXml(Toolbar4);
    QVERIFY2(ok, "toolbar4.xml was not loaded correctly");

    QVERIFY(subject->isVisible() == false);
}

void Ut_MToolbarData::testRefuseAttribute_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QStringList>("expectedValue");

    QTest::newRow("toolbar1") << Toolbar1 << (QStringList() << "_close");
    QTest::newRow("toolbar2") << Toolbar2 << QStringList();
    QTest::newRow("toolbar3") << Toolbar3 << (QStringList() << "_close" << "_copypaste");
}

void Ut_MToolbarData::testRefuseAttribute()
{
    QFETCH(QString, fileName);
    QFETCH(QStringList, expectedValue);

    QVERIFY2(QFile(fileName).exists(), "xml file does not exist");
    bool ok;

    ok = subject->loadToolbarXml(fileName);
    QVERIFY2(ok, "toolbar was not loaded correctly");

    QCOMPARE(subject->refusedNames(), expectedValue);
}

void Ut_MToolbarData::testAddItem()
{
    QVERIFY2(QFile(Toolbar1).exists(), "toolbar1.xml does not exist");
    bool ok;
    QSharedPointer<MToolbarItem> item;
    QSharedPointer<MToolbarItem> button(new MToolbarItem("addItem", MInputMethod::ItemButton));

    button->setAlignment(Qt::AlignLeft);

    ok = subject->append(QSharedPointer<MToolbarLayout>(new MToolbarLayout), button);
    QVERIFY2(!ok, "Item should not be added to non-constructed toolbar");

    ok = subject->loadToolbarXml(Toolbar1);
    QVERIFY2(ok, "toolbar1.xml was not loaded correctly");

    QSharedPointer<MToolbarLayout> landscape = subject->layout(MInputMethod::Landscape).constCast<MToolbarLayout>();
    QVERIFY(!landscape.isNull());
    QVERIFY(!landscape->items().isEmpty());

    ok = subject->append(landscape, button);
    QVERIFY2(ok, "Item was not added");
    QVERIFY(landscape->items().last() == button);

    ok = subject->append(landscape, button);
    QVERIFY2(!ok, "Item should not be added twice to the same row");

    ok = subject->append(QSharedPointer<MToolbarLayout>(), button);
    QVERIFY2(!ok, "Item should not be added to non-existing row");

    QSharedPointer<MToolbarLayout> portrait = subject->layout(MInputMethod::Portrait).constCast<MToolbarLayout>();
    QVERIFY(!portrait.isNull());
    QVERIFY(!portrait->items().isEmpty());

    ok = subject->append(portrait, button);
    QVERIFY2(ok, "Item was not added");
    QVERIFY(portrait->items().last() == button);

    ok = subject->append(portrait, button);
    QVERIFY2(!ok, "Item should not be added twice to the same row");
}

QTEST_APPLESS_MAIN(Ut_MToolbarData)

