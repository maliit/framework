/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "variantmarshalling.h"

#include <QVariant>
#include <QString>
#include <QRect>
#include <QDebug>
#include <QStringList>

namespace
{
    void destroyGValue(GValue *value)
    {
        g_value_unset(value);
        g_free(value);
    }
}

bool decodeVariant(QVariant *dest, GValue *source, QString *error_message)
{
    switch (G_VALUE_TYPE(source)) {
    case G_TYPE_BOOLEAN:
        *dest = bool(g_value_get_boolean(source));
        return true;
    case G_TYPE_INT:
        *dest = int(g_value_get_int(source));
        return true;
    case G_TYPE_UINT:
        *dest = uint(g_value_get_uint(source));
        return true;
    case G_TYPE_INT64:
        *dest = static_cast<qlonglong>(g_value_get_int64(source));
        return true;
    case G_TYPE_UINT64:
        *dest = static_cast<qulonglong>(g_value_get_uint64(source));
        return true;
    case G_TYPE_DOUBLE:
        *dest = double(g_value_get_double(source));
        return true;
    case G_TYPE_STRING:
        *dest = QString::fromUtf8(g_value_get_string(source));
        return true;
    default:
        // The encoding must to be compatible with QDBusArgument's encoding of types,
        // because the implementation will be eventually changed to using the QDBus.
        //
        // Unfortunately, the QDBusArgument encoding of variants
        // does not encode information on the type explicitly,
        // and we have to recover types from structures.
        //
        // For instance, (iiii) means QRect.
        // (QVariant which holds array of four integers cannot be converted to QRect)
        if (G_VALUE_TYPE(source) == dbus_g_type_get_struct("GValueArray",
                                                           G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT,
                                                           G_TYPE_INVALID)) {
            int left;
            int top;
            int width;
            int height;
            if (!dbus_g_type_struct_get(source,
                                        0, &left, 1, &top, 2, &width, 3, &height,
                                        G_MAXUINT)) {
                gchar *contents = g_strdup_value_contents(source);
                if (error_message != 0)
                    *error_message = QString(": failed to extract Rect from: ") + contents;
                g_free(contents);
                return false;
            }
            *dest = QRect(left, top, width, height);
            return true;
        } else if (G_VALUE_TYPE(source) == G_TYPE_STRV) {
            QStringList strings;

            gchar **array = (gchar**)g_value_get_boxed(source);

            for (guint i = 0; array[i]; ++i) {
                strings.append(QString::fromUtf8(array[i]));
            }

            *dest = strings;
            return true;
        } else if (G_VALUE_TYPE(source) == G_TYPE_VALUE) {
            return decodeVariant(dest, (GValue*)g_value_get_boxed(source), error_message);
        } else {
            if (error_message != 0)
                *error_message = QString(": unknown data type: ") + G_VALUE_TYPE_NAME(source);
            return false;
        }
    }
}

bool decodeVariantMap(QVariantMap *dest, GHashTable *source, QString *error_message)
{
    dest->clear();
    GHashTableIter iterator;
    gchar *keyData;
    GValue *valueData;
    g_hash_table_iter_init(&iterator, source);
    while (g_hash_table_iter_next(&iterator, (void**)(&keyData), (void**)(&valueData))) {
        QString key = QString::fromUtf8(keyData);
        QVariant value;
        if (!decodeVariant(&value, valueData, error_message)) {
            if (error_message != 0)
                *error_message = "[\"" + key + "\"]" + *error_message;
            return false;
        }

        dest->insert(key, value);
    }

    return true;
}

bool encodeVariant(GValue *dest, const QVariant &source)
{
    switch (static_cast<QMetaType::Type>(source.type())) {
    case QMetaType::Bool:
        g_value_init(dest, G_TYPE_BOOLEAN);
        g_value_set_boolean(dest, source.toBool());
        return true;
    case QMetaType::Int:
        g_value_init(dest, G_TYPE_INT);
        g_value_set_int(dest, source.toInt());
        return true;
    case QMetaType::UInt:
        g_value_init(dest, G_TYPE_UINT);
        g_value_set_uint(dest, source.toUInt());
        return true;
    case QMetaType::LongLong:
        g_value_init(dest, G_TYPE_INT64);
        g_value_set_int64(dest, source.toLongLong());
        return true;
    case QMetaType::ULongLong:
        g_value_init(dest, G_TYPE_UINT64);
        g_value_set_uint64(dest, source.toULongLong());
        return true;
    case QMetaType::Double:
        g_value_init(dest, G_TYPE_DOUBLE);
        g_value_set_double(dest, source.toDouble());
        return true;
    case QMetaType::QString:
        g_value_init(dest, G_TYPE_STRING);
        // string is copied by g_value_set_string
        g_value_set_string(dest, source.toString().toUtf8().constData());
        return true;
    case QMetaType::QRect:
        {
            // QRect is encoded as (iiii).
            // This is compatible with QDBusArgument encoding. see more in decoder
            GType structType = dbus_g_type_get_struct("GValueArray",
                                                      G_TYPE_INT, G_TYPE_INT,
                                                      G_TYPE_INT, G_TYPE_INT,
                                                      G_TYPE_INVALID);
            g_value_init(dest, structType);
            GValueArray *array = (GValueArray*)dbus_g_type_specialized_construct(structType);
            if (!array) {
                qWarning() << Q_FUNC_INFO << "failed to initialize Rect instance";
            }
            g_value_take_boxed(dest, array);
            QRect rect = source.toRect();
            if (!dbus_g_type_struct_set(dest,
                                        0, rect.left(),
                                        1, rect.top(),
                                        2, rect.width(),
                                        3, rect.height(),
                                        G_MAXUINT)) {
                g_value_unset(dest);
                qWarning() << Q_FUNC_INFO << "failed to fill Rect instance";
                return false;
            }
            return true;
        }
    case QMetaType::ULong:
        g_value_init(dest, G_TYPE_ULONG);
        g_value_set_ulong(dest, source.value<ulong>());
        return true;
    case QMetaType::QStringList:
        {
            QStringList strings = source.toStringList();
            gchar **array = g_new(gchar *, strings.count() + 1);

            array[strings.count()] = 0;

            g_value_init(dest, G_TYPE_STRV);
            g_value_take_boxed(dest, array);

            Q_FOREACH (const QString &string, source.toStringList()) {
                gchar *dup = g_strdup(string.toUtf8().data());

                *array++ = dup;
            }
            return true;
        }
    case QMetaType::Void:
        g_value_init(dest, G_TYPE_INVALID);
        return true;
    default:
        qWarning() << Q_FUNC_INFO << "unsupported data:" << source.type() << source;
        return false;
    }
}

GHashTable *encodeVariantMap(const QMap<QString, QVariant> &source)
{
    GHashTable* result = g_hash_table_new_full(&g_str_hash, &g_str_equal,
                                               &g_free, GDestroyNotify(&destroyGValue));
    Q_FOREACH (QString key, source.keys()) {
        GValue *valueVariant = g_new0(GValue, 1);
        if (!encodeVariant(valueVariant, source[key])) {
            g_free(valueVariant);
            g_hash_table_unref(result);
            return 0;
        }
        g_hash_table_insert(result, g_strdup(key.toUtf8().constData()), valueVariant);
    }
    return result;
}

