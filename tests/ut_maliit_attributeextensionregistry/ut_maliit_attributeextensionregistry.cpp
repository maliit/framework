#include <QtCore/QString>
#include <QtTest/QtTest>

#include <maliit/attributeextensionregistry.h>
#include <tr1/memory>

using Maliit::AttributeExtensionRegistry;
using Maliit::AttributeExtension;

typedef std::tr1::shared_ptr<Maliit::AttributeExtension> SharedAttributeExtension;

namespace {
    SharedAttributeExtension createDefault()
    {
        return SharedAttributeExtension(new Maliit::AttributeExtension(QString()));
    }

    int id(const QWeakPointer<AttributeExtension> &ptr)
    {
        AttributeExtension *extension = ptr.data();
        if (extension)
            return extension->id();

        return -1;
    }
}

class Ut_Maliit_AttributeExtensionRegistry : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testDefault();
    void testAddExtension();
    void testRemoveExtension();
    void testExtensionRegisteredSignal();
    void testExtensionUnregisteredSignal();
    void testExtensionChangedSignal();
};

void Ut_Maliit_AttributeExtensionRegistry::testDefault()
{
    QVERIFY(AttributeExtensionRegistry::instance()->extensions().empty());
}

void Ut_Maliit_AttributeExtensionRegistry::testAddExtension()
{
    SharedAttributeExtension extension(createDefault());

    QCOMPARE(AttributeExtensionRegistry::instance()->extensions().size(), 1);
    QCOMPARE(id(AttributeExtensionRegistry::instance()->extensions().first()), extension->id());
}

void Ut_Maliit_AttributeExtensionRegistry::testRemoveExtension()
{
    SharedAttributeExtension extension(createDefault());

    extension.reset();

    QCOMPARE(AttributeExtensionRegistry::instance()->extensions().size(), 0);
}

void Ut_Maliit_AttributeExtensionRegistry::testExtensionRegisteredSignal()
{
    QSignalSpy spy(AttributeExtensionRegistry::instance(), SIGNAL(extensionRegistered(int,QString)));

    SharedAttributeExtension extension(createDefault());

    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy.first().at(0).toInt(), extension->id());
    QCOMPARE(spy.first().at(1).toString(), extension->fileName());
}

void Ut_Maliit_AttributeExtensionRegistry::testExtensionUnregisteredSignal()
{
    QSignalSpy spy(AttributeExtensionRegistry::instance(), SIGNAL(extensionUnregistered(int)));

    SharedAttributeExtension extension(createDefault());
    int id = extension->id();
    extension.reset();

    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy.first().at(0).toInt(), id);
}

void Ut_Maliit_AttributeExtensionRegistry::testExtensionChangedSignal()
{
    QSignalSpy spy(AttributeExtensionRegistry::instance(), SIGNAL(extensionChanged(int,QString,QVariant)));

    const QString &key = QString::fromLatin1("TestKey");
    const QVariant &value(QString::fromLatin1("TestValue"));

    SharedAttributeExtension extension(createDefault());
    extension->setAttribute(key, value);

    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy.first().at(0).toInt(), extension->id());
    QCOMPARE(spy.first().at(1).toString(), key);
    QCOMPARE(spy.first().at(2).value<QVariant>(), value);
}

QTEST_APPLESS_MAIN(Ut_Maliit_AttributeExtensionRegistry)

#include "ut_maliit_attributeextensionregistry.moc"
