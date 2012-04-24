#include <QtCore/QString>
#include <QtTest/QtTest>

#include <maliit/settingdata.h>

class Ut_Maliit_Settings : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testBoolValidation_data();
    void testIntValidation_data();
    void testIntRangeValidation_data();
    void testStringValidation_data();
    void testStringListValidation_data();
    void testIntListValidation_data();

    void testBoolValidation();
    void testIntValidation();
    void testIntRangeValidation();
    void testStringValidation();
    void testStringListValidation();
    void testIntListValidation();
};

void Ut_Maliit_Settings::testBoolValidation_data()
{
    QVariantMap noattr;

    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QVariantMap>("attributes");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("false") << QVariant(false) << noattr << true;
    QTest::newRow("true") << QVariant(true) << noattr << true;
    QTest::newRow("zero") << QVariant(0) << noattr << true;
    QTest::newRow("empty variant") << QVariant() << noattr << false;
}

void Ut_Maliit_Settings::testBoolValidation()
{
    QFETCH(QVariant, value);
    QFETCH(QVariantMap, attributes);
    QFETCH(bool, isValid);

    QCOMPARE(validateSettingValue(Maliit::BoolType, attributes, value), isValid);
}

void Ut_Maliit_Settings::testIntValidation_data()
{
    QVariantList domainValues = QVariantList() << 1 << 2;
    QVariantMap noattr, domain;

    domain[Maliit::SettingEntryAttributes::valueDomain] = domainValues;

    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QVariantMap>("attributes");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("zero") << QVariant(0) << noattr << true;
    QTest::newRow("int") << QVariant(42) << noattr << true;
    QTest::newRow("false") << QVariant(false) << noattr << true;
    QTest::newRow("numeric string") << QVariant("42") << noattr << true;
    QTest::newRow("non-numeric string") << QVariant("ook") << noattr << false;
    QTest::newRow("empty variant") << QVariant() << noattr << false;

    QTest::newRow("int in domain") << QVariant(2) << domain << true;
    QTest::newRow("int outside domain") << QVariant(3) << domain << false;
}

void Ut_Maliit_Settings::testIntValidation()
{
    QFETCH(QVariant, value);
    QFETCH(QVariantMap, attributes);
    QFETCH(bool, isValid);

    QCOMPARE(validateSettingValue(Maliit::IntType, attributes, value), isValid);
}

void Ut_Maliit_Settings::testIntRangeValidation_data()
{
    QVariantMap range_min, range_max, range_min_max;

    range_min[Maliit::SettingEntryAttributes::valueRangeMin] = 3;
    range_max[Maliit::SettingEntryAttributes::valueRangeMax] = 5;
    range_min_max[Maliit::SettingEntryAttributes::valueRangeMin] = 3;
    range_min_max[Maliit::SettingEntryAttributes::valueRangeMax] = 5;

    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QVariantMap>("attributes");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("minrange, in range") << QVariant(6) << range_min << true;
    QTest::newRow("minrange, below range") << QVariant(2) << range_min << false;
    QTest::newRow("minrange, at min") << QVariant(3) << range_min << true;

    QTest::newRow("maxrange, in range") << QVariant(1) << range_max << true;
    QTest::newRow("maxrange, above range") << QVariant(6) << range_max << false;
    QTest::newRow("maxrange, at max") << QVariant(5) << range_max << true;

    QTest::newRow("minmaxrange, in range") << QVariant(4) << range_min_max << true;
    QTest::newRow("minmaxrange, above range") << QVariant(6) << range_min_max << false;
    QTest::newRow("minmaxrange, below range") << QVariant(1) << range_min_max << false;
    QTest::newRow("minmaxrange, at min") << QVariant(3) << range_min_max << true;
    QTest::newRow("minmaxrange, at max") << QVariant(5) << range_min_max << true;
}

void Ut_Maliit_Settings::testIntRangeValidation()
{
    QFETCH(QVariant, value);
    QFETCH(QVariantMap, attributes);
    QFETCH(bool, isValid);

    QCOMPARE(validateSettingValue(Maliit::IntType, attributes, value), isValid);
}

void Ut_Maliit_Settings::testStringValidation_data()
{
    QVariantList domainValues = QVariantList() << "a" << "b";
    QVariantMap noattr, domain;

    domain[Maliit::SettingEntryAttributes::valueDomain] = domainValues;

    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QVariantMap>("attributes");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("empty string") << QVariant("") << noattr << true;
    QTest::newRow("string") << QVariant("aaa") << noattr << true;
    QTest::newRow("false") << QVariant(false) << noattr << true;
    QTest::newRow("zero") << QVariant(0) << noattr << true;
    QTest::newRow("empty variant") << QVariant() << noattr << false;

    QTest::newRow("string in domain") << QVariant("b") << domain << true;
    QTest::newRow("string outside domain") << QVariant("c") << domain << false;
}

void Ut_Maliit_Settings::testStringValidation()
{
    QFETCH(QVariant, value);
    QFETCH(QVariantMap, attributes);
    QFETCH(bool, isValid);

    QCOMPARE(validateSettingValue(Maliit::StringType, attributes, value), isValid);
}

void Ut_Maliit_Settings::testStringListValidation_data()
{
    QVariantList domainValues = QVariantList() << "a" << "b";
    QVariantMap noattr, domain;

    domain[Maliit::SettingEntryAttributes::valueDomain] = domainValues;

    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QVariantMap>("attributes");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("empty string list") << QVariant(QStringList()) << noattr << true;
    QTest::newRow("string list") << QVariant(QStringList() << "a" << "b") << noattr << true;
    QTest::newRow("variant list with int and strings") << QVariant(QVariantList() << "a" << 2) << noattr << true;
    QTest::newRow("single value") << QVariant(1) << noattr << false;
    QTest::newRow("empty variant") << QVariant() << noattr << false;

    QTest::newRow("strings in domain") << QVariant(QStringList() << "a" << "b" << "a" << "a" << "b") << domain << true;
    QTest::newRow("strings outside domain") << QVariant(QStringList() << "a" << "b" << "a" << "c" << "b") << domain << false;
}

void Ut_Maliit_Settings::testStringListValidation()
{
    QFETCH(QVariant, value);
    QFETCH(QVariantMap, attributes);
    QFETCH(bool, isValid);

    QCOMPARE(validateSettingValue(Maliit::StringListType, attributes, value), isValid);
}

void Ut_Maliit_Settings::testIntListValidation_data()
{
    QVariantList domainValues = QVariantList() << 1 << 2;
    QVariantMap noattr, domain, range_min_max;

    domain[Maliit::SettingEntryAttributes::valueDomain] = domainValues;

    range_min_max[Maliit::SettingEntryAttributes::valueRangeMin] = 3;
    range_min_max[Maliit::SettingEntryAttributes::valueRangeMax] = 5;

    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QVariantMap>("attributes");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("empty list") << QVariant(QVariantList()) << noattr << true;
    QTest::newRow("int list") << QVariant(QVariantList() << 1 << 2) << noattr << true;
    QTest::newRow("numeric string list") << QVariant(QStringList() << "1" << "2") << noattr << true;
    QTest::newRow("mixed int/string") << QVariant(QVariantList() << "a" << 2) << noattr << false;
    QTest::newRow("single int value") << QVariant(1) << noattr << false;
    QTest::newRow("single int value") << QVariant("a") << noattr << false;
    QTest::newRow("empty variant") << QVariant() << noattr << false;

    QTest::newRow("int in domain") << QVariant(QVariantList() << 1 << 2 << 1 << 1 << 2) << domain << true;
    QTest::newRow("int outside domain") << QVariant(QVariantList() << 1 << 2 << 1 << 3 << 2) << domain << false;

    QTest::newRow("int in range") << QVariant(QVariantList() << 3 << 4 << 5) << range_min_max << true;
    QTest::newRow("int outside range 1") << QVariant(QVariantList() << 3 << 4 << 5 << 1) << range_min_max << false;
    QTest::newRow("int outside range 2") << QVariant(QVariantList() << 3 << 4 << 5 << 6) << range_min_max << false;
}

void Ut_Maliit_Settings::testIntListValidation()
{
    QFETCH(QVariant, value);
    QFETCH(QVariantMap, attributes);
    QFETCH(bool, isValid);

    QCOMPARE(validateSettingValue(Maliit::IntListType, attributes, value), isValid);
}

QTEST_APPLESS_MAIN(Ut_Maliit_Settings)

#include "ut_maliit_settings.moc"
