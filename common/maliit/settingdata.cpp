/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2012 Mattia Barbon <mattia@develer.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "maliit/settingdata.h"

namespace
{
    bool checkValueDomain(const QVariant &value, const QVariant &domain)
    {
        if (!domain.isValid())
            return true;
        if (!domain.canConvert(QVariant::List))
            return false;

        QVariantList domain_values = domain.toList();

        return domain_values.contains(value);
    }

    bool checkValueRange(const QVariant &value, const QVariant &range_min, const QVariant &range_max)
    {
        if (!range_min.isValid() && !range_max.isValid())
            return true;

        if (range_min.isValid()) {
            if (!range_min.canConvert(QVariant::Int))
                return false;
            if (range_min.toInt() > value.toInt())
                return false;
        }

        if (range_max.isValid()) {
            if (!range_max.canConvert(QVariant::Int))
                return false;
            if (range_max.toInt() < value.toInt())
                return false;
        }

        return true;
    }

    bool checkValueDomain(const QVariantList &values, const QVariant &domain)
    {
        if (!domain.isValid())
            return true;
        if (!domain.canConvert(QVariant::List))
            return false;

        const QVariantList &domain_values = domain.toList();

        Q_FOREACH (const QVariant &v, values)
            if (!domain_values.contains(v))
                return false;

        return true;
    }

    bool checkValueRange(const QVariantList &values, const QVariant &range_min, const QVariant &range_max)
    {
        if (!range_min.isValid() && !range_max.isValid())
            return true;

        Q_FOREACH (const QVariant &v, values)
            if (!checkValueRange(v, range_min, range_max))
                return false;

        return true;
    }

    bool checkIntList(const QVariant &value)
    {
        if (!value.canConvert<QVariantList>())
            return false;

        const QVariantList &values = value.toList();

        Q_FOREACH (const QVariant &v, values)
        {
            QVariant copy = v;

            if (!v.canConvert<int>() || !copy.convert(QVariant::Int))
                return false;
        }

        return true;
    }
}

bool validateSettingValue(Maliit::SettingEntryType type, const QVariantMap attributes, const QVariant &value)
{
    QVariant domain = attributes[Maliit::SettingEntryAttributes::valueDomain];
    QVariant range_min = attributes[Maliit::SettingEntryAttributes::valueRangeMin];
    QVariant range_max = attributes[Maliit::SettingEntryAttributes::valueRangeMax];
    QVariant copy = value;

    switch (type)
    {
    case Maliit::StringType:
        if (!value.canConvert<QString>())
            return false;
        if (!checkValueDomain(value, domain))
            return false;
        break;
    case Maliit::IntType:
        if (!value.canConvert<int>() || !copy.convert(QVariant::Int))
            return false;
        if (!checkValueDomain(value, domain))
            return false;
        if (!checkValueRange(value, range_min, range_max))
            return false;
        break;
    case Maliit::BoolType:
        if (!value.canConvert<bool>())
            return false;
        break;
    case Maliit::StringListType:
        if (!value.canConvert<QStringList>())
            return false;
        if (!checkValueDomain(value.toList(), domain))
            return false;
        break;
    case Maliit::IntListType:
        if (!checkIntList(value))
            return false;
        if (!checkValueDomain(value.toList(), domain))
            return false;
        if (!checkValueRange(value.toList(), range_min, range_max))
            return false;
        break;
    }

    return true;
}
