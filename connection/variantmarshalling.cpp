/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
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
        } else {
            if (error_message != 0)
                *error_message = QString(": unknown data type: ") + G_VALUE_TYPE_NAME(source);
            return false;
        }
    }
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
    default:
        qWarning() << Q_FUNC_INFO << "unsupported data:" << source.type() << source;
        return false;
    }
}

