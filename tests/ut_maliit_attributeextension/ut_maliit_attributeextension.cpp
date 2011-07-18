#include <QtCore/QString>
#include <QtTest/QtTest>

#include <maliit/attributeextension.h>
#include <tr1/memory>
#include <algorithm>

typedef std::tr1::shared_ptr<Maliit::AttributeExtension> SharedAttributeExtension;

typedef std::vector<std::pair<QString, QVariant> > KeyValueContainer;

Q_DECLARE_METATYPE(KeyValueContainer)

namespace {
    SharedAttributeExtension createDefault()
    {
        return SharedAttributeExtension(new Maliit::AttributeExtension(QString()));
    }

    int id(const SharedAttributeExtension &ptr)
    {
        return ptr->id();
    }

    template<typename Container>
    bool containsNoDuplicates(const Container &container)
    {
        std::vector<typename Container::value_type> copy(container.begin(), container.end());
        std::sort(copy.begin(), copy.end());
        return std::unique(copy.begin(), copy.end()) == copy.end();
    }

    struct SetAttribute {
        explicit SetAttribute(const SharedAttributeExtension &newExtension) :
            extension(newExtension)
        {}

        void operator() (const std::pair<QString, QVariant> &keyValue)
        {
            extension->setAttribute(keyValue.first, keyValue.second);
        }

    private:
        SharedAttributeExtension extension;
    };

    struct CompareAttribute {
        explicit CompareAttribute(const SharedAttributeExtension &newExtension) :
            extension(newExtension)
        {}

        void operator() (const std::pair<QString, QVariant> &keyValue)
        {
            QCOMPARE(extension->attributes().value(keyValue.first), keyValue.second);
        }

    private:
        SharedAttributeExtension extension;
    };

    void compareAttributes(const SharedAttributeExtension &extension, const KeyValueContainer &result)
    {
        QCOMPARE(extension->attributes().size(), static_cast<int>(result.size()));
        std::for_each(result.begin(), result.end(), CompareAttribute(extension));
    }
}

class Ut_Maliit_AttributeExtension : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testDefaultFilename();
    void testDefaultAttributes();
    void testFilename();
    void testIdsUnique();
    void testAttributes();
    void testAttributes_data();
};

void Ut_Maliit_AttributeExtension::testDefaultFilename()
{
    const SharedAttributeExtension subject = createDefault();

    QCOMPARE(subject->fileName(), QString());
}

void Ut_Maliit_AttributeExtension::testDefaultAttributes()
{
    const SharedAttributeExtension subject = createDefault();

    QVERIFY(subject->attributes().empty());
}

void Ut_Maliit_AttributeExtension::testFilename()
{
    const QString testName = QString::fromLatin1("TestName");
    const SharedAttributeExtension subject(new Maliit::AttributeExtension(testName));

    QCOMPARE(subject->fileName(), testName);
}

void Ut_Maliit_AttributeExtension::testIdsUnique()
{
    std::vector<SharedAttributeExtension> subjects(10);
    std::generate(subjects.begin(), subjects.end(), &createDefault);

    std::vector<int> ids(subjects.size());
    std::transform(subjects.begin(), subjects.end(), ids.begin(), &id);

    QVERIFY(containsNoDuplicates(ids));
}

void Ut_Maliit_AttributeExtension::testAttributes()
{
    QFETCH(KeyValueContainer, attributes);
    QFETCH(KeyValueContainer, result);

    const SharedAttributeExtension subject = createDefault();
    std::for_each(attributes.begin(), attributes.end(), SetAttribute(subject));

    compareAttributes(subject, result);
}

void Ut_Maliit_AttributeExtension::testAttributes_data()
{
    QTest::addColumn<KeyValueContainer>("attributes");
    QTest::addColumn<KeyValueContainer>("result");

    KeyValueContainer attributes;
    attributes.push_back(std::make_pair(QString::fromLatin1("TestKey1"), QVariant(1000)));
    attributes.push_back(std::make_pair(QString::fromLatin1("TestKey2"), QVariant(2000)));
    attributes.push_back(std::make_pair(QString::fromLatin1("TestKey3"), QVariant(3000)));
    KeyValueContainer result(attributes.begin(), attributes.end());

    QTest::newRow("add") << attributes << result;

    attributes.push_back(std::make_pair(QString::fromLatin1("TestKey1"), QVariant(1100)));
    attributes.push_back(std::make_pair(QString::fromLatin1("TestKey2"), QVariant(2000)));
    attributes.push_back(std::make_pair(QString::fromLatin1("TestKey3"), QVariant(3000)));

    result.assign(attributes.begin() + 3, attributes.end());

    QTest::newRow("change") << attributes << result;

    // One cannot remove attributes just override them with an empty value
    attributes.push_back(std::make_pair(QString::fromLatin1("TestKey1"), QVariant()));
    attributes.push_back(std::make_pair(QString::fromLatin1("TestKey2"), QVariant()));

    result.assign(attributes.begin() + 5, attributes.end());

    QTest::newRow("remove") << attributes << result;
}

QTEST_APPLESS_MAIN(Ut_Maliit_AttributeExtension)

#include "ut_maliit_attributeextension.moc"
