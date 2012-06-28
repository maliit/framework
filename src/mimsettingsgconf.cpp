/* * This file is part of maliit-framework *
 *
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mimsettingsgconf.h"

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QVariant>
#include <QDebug>

#include <gconf/gconf-value.h>
#include <gconf/gconf-client.h>

struct MImSettingsGConfBackendPrivate {
    MImSettingsGConfBackendPrivate() :
        notify_id(0),
        have_gconf(false)
    {}

    QString key;
    QVariant value;
    guint notify_id;
    bool have_gconf;

    static void notify_trampoline(GConfClient *, guint, GConfEntry *, gpointer);
};

/* We get the default client and never release it, on purpose, to
   avoid disconnecting from the GConf daemon when a program happens to
   not have any GConfItems for short periods of time.
 */
static GConfClient *
get_gconf_client ()
{
    static GConfClient *s_gconf_client = 0;
    struct GConfClientDestroyer {
        ~GConfClientDestroyer() { g_object_unref(s_gconf_client); s_gconf_client = 0; }
    };

    static GConfClientDestroyer gconfClientDestroyer;
    if (s_gconf_client)
        return s_gconf_client;

    g_type_init();
    s_gconf_client = gconf_client_get_default();

    return s_gconf_client;
}


#define withClient(c) for (GConfClient *c = get_gconf_client (); c; c = NULL)

static QByteArray convertKey(const QString &key)
{
    if (key.startsWith('/'))
        return key.toUtf8();
    else {
        QString replaced = key;
        replaced.replace('.', '/');
        qWarning() << "mgconfitem.cpp" << "Using dot-separated key names with MImSettingsGConfBackend is deprecated.";
        qWarning() << "mgconfitem.cpp" << "Please use" << '/' + replaced << "instead of" << key;
        return '/' + replaced.toUtf8();
    }
}

static QString convertKey(const char *key)
{
    return QString::fromUtf8(key);
}

static QVariant convertValue(GConfValue *src)
{
    if (!src) {
        return QVariant();
    } else {
        switch (src->type) {
        case GCONF_VALUE_INVALID:
            return QVariant(QVariant::Invalid);
        case GCONF_VALUE_BOOL:
            return QVariant((bool)gconf_value_get_bool(src));
        case GCONF_VALUE_INT:
            return QVariant(gconf_value_get_int(src));
        case GCONF_VALUE_FLOAT:
            return QVariant(gconf_value_get_float(src));
        case GCONF_VALUE_STRING:
            return QVariant(QString::fromUtf8(gconf_value_get_string(src)));
        case GCONF_VALUE_LIST:
            switch (gconf_value_get_list_type(src)) {
            case GCONF_VALUE_STRING: {
                QStringList result;
                for (GSList *elts = gconf_value_get_list(src); elts; elts = elts->next)
                    result.append(QString::fromUtf8(gconf_value_get_string((GConfValue *)elts->data)));
                return QVariant(result);
            }
            default: {
                QList<QVariant> result;
                for (GSList *elts = gconf_value_get_list(src); elts; elts = elts->next)
                    result.append(convertValue((GConfValue *)elts->data));
                return QVariant(result);
            }
            }
        case GCONF_VALUE_SCHEMA:
        default:
            return QVariant();
        }
    }
}

static GConfValue *convertString(const QString &str)
{
    GConfValue *v = gconf_value_new(GCONF_VALUE_STRING);
    gconf_value_set_string(v, str.toUtf8().data());
    return v;
}

static GConfValueType primitiveType(const QVariant &elt)
{
    switch (elt.type()) {
    case QVariant::String:
        return GCONF_VALUE_STRING;
    case QVariant::Int:
        return GCONF_VALUE_INT;
    case QVariant::Double:
        return GCONF_VALUE_FLOAT;
    case QVariant::Bool:
        return GCONF_VALUE_BOOL;
    default:
        return GCONF_VALUE_INVALID;
    }
}

static GConfValueType uniformType(const QList<QVariant> &list)
{
    GConfValueType result = GCONF_VALUE_INVALID;

    Q_FOREACH(const QVariant & elt, list) {
        GConfValueType elt_type = primitiveType(elt);

        if (elt_type == GCONF_VALUE_INVALID)
            return GCONF_VALUE_INVALID;

        if (result == GCONF_VALUE_INVALID)
            result = elt_type;
        else if (result != elt_type)
            return GCONF_VALUE_INVALID;
    }

    if (result == GCONF_VALUE_INVALID)
        return GCONF_VALUE_STRING;  // empty list.
    else
        return result;
}

static int convertValue(const QVariant &src, GConfValue **valp)
{
    GConfValue *v;

    switch (src.type()) {
    case QVariant::Invalid:
        v = NULL;
        break;
    case QVariant::Bool:
        v = gconf_value_new(GCONF_VALUE_BOOL);
        gconf_value_set_bool(v, src.toBool());
        break;
    case QVariant::Int:
        v = gconf_value_new(GCONF_VALUE_INT);
        gconf_value_set_int(v, src.toInt());
        break;
    case QVariant::Double:
        v = gconf_value_new(GCONF_VALUE_FLOAT);
        gconf_value_set_float(v, src.toDouble());
        break;
    case QVariant::String:
        v = convertString(src.toString());
        break;
    case QVariant::StringList: {
        GSList *elts = NULL;
        v = gconf_value_new(GCONF_VALUE_LIST);
        gconf_value_set_list_type(v, GCONF_VALUE_STRING);
        Q_FOREACH(const QString & str, src.toStringList())
        elts = g_slist_prepend(elts, convertString(str));
        gconf_value_set_list_nocopy(v, g_slist_reverse(elts));
        break;
    }
    case QVariant::List: {
        GConfValueType elt_type = uniformType(src.toList());
        if (elt_type == GCONF_VALUE_INVALID)
            v = NULL;
        else {
            GSList *elts = NULL;
            v = gconf_value_new(GCONF_VALUE_LIST);
            gconf_value_set_list_type(v, elt_type);
            Q_FOREACH(const QVariant & elt, src.toList()) {
                GConfValue *val = NULL;
                convertValue(elt, &val);  // guaranteed to succeed.
                elts = g_slist_prepend(elts, val);
            }
            gconf_value_set_list_nocopy(v, g_slist_reverse(elts));
        }
        break;
    }
    default:
        return 0;
    }

    *valp = v;
    return 1;
}

void MImSettingsGConfBackendPrivate::notify_trampoline(GConfClient *,
        guint,
        GConfEntry *,
        gpointer data)
{
    MImSettingsGConfBackend *item = (MImSettingsGConfBackend *)data;
    item->update_value(true);
}

void MImSettingsGConfBackend::update_value(bool emit_signal)
{
    Q_D(MImSettingsGConfBackend);
    QVariant new_value;

    withClient(client) {
        GError *error = NULL;
        QByteArray k = convertKey(d->key);
        GConfValue *v = gconf_client_get(client, k.data(), &error);

        if (error) {
            qWarning() << "MImSettingsGConfBackend" << error->message;
            g_error_free(error);
            new_value = d->value;
        } else {
            new_value = convertValue(v);
            if (v)
                gconf_value_free(v);
        }
    }

    if (new_value != d->value) {
        d->value = new_value;
        if (emit_signal)
            Q_EMIT valueChanged();
    }
}

QString MImSettingsGConfBackend::key() const
{
    Q_D(const MImSettingsGConfBackend);

    return d->key;
}

QVariant MImSettingsGConfBackend::value(const QVariant &def) const
{
    Q_D(const MImSettingsGConfBackend);

    if (d->value.isNull())
        return def;
    else
        return d->value;
}

void MImSettingsGConfBackend::set(const QVariant &val)
{
    Q_D(MImSettingsGConfBackend);

    withClient(client) {
        QByteArray k = convertKey(d->key);
        GConfValue *v;
        if (convertValue(val, &v)) {
            GError *error = NULL;

            if (v) {
                gconf_client_set(client, k.data(), v, &error);
                gconf_value_free(v);
            } else {
                gconf_client_unset(client, k.data(), &error);
            }

            if (error) {
                qWarning() << "MImSettingsGConfBackend" << error->message;
                g_error_free(error);
            } else if (d->value != val) {
                d->value = val;
                Q_EMIT valueChanged();
            }

        } else
            qWarning() << "MImSettingsGConfBackend" << "Can't store a" << val.typeName();
    }
}

void MImSettingsGConfBackend::unset()
{
    set(QVariant());
}

QList<QString> MImSettingsGConfBackend::listDirs() const
{
    Q_D(const MImSettingsGConfBackend);
    QList<QString> children;

    withClient(client) {
        QByteArray k = convertKey(d->key);
        GError *error = NULL;
        GSList *dirs = gconf_client_all_dirs(client, k.data(), &error);
        if(error) {
            qWarning() << "MImSettingsGConfBackend" << error->message;
            g_error_free(error);
            return children;
        }

        for (GSList *d = dirs; d; d = d->next) {
            children.append(convertKey((char *)d->data));
            g_free(d->data);
        }
        g_slist_free(dirs);
    }

    return children;
}

QList<QString> MImSettingsGConfBackend::listEntries() const
{
    Q_D(const MImSettingsGConfBackend);
    QList<QString> children;

    withClient(client) {
        QByteArray k = convertKey(d->key);
        GError *error = NULL;
        GSList *entries = gconf_client_all_entries(client, k.data(), &error);
        if(error) {
            qWarning() << "MImSettingsGConfBackend" << error->message;
            g_error_free(error);
            return children;
        }

        for (GSList *e = entries; e; e = e->next) {
            children.append(convertKey(((GConfEntry *)e->data)->key));
            gconf_entry_free((GConfEntry *)e->data);
        }
        g_slist_free(entries);
    }

    return children;
}

MImSettingsGConfBackend::MImSettingsGConfBackend(const QString &key, QObject *parent) :
    MImSettingsBackend(parent),
    d_ptr(new MImSettingsGConfBackendPrivate)
{
    Q_D(MImSettingsGConfBackend);

    d->key = key;
    withClient(client) {
        QByteArray k = convertKey(d->key);
        GError *error = NULL;

        int index = k.lastIndexOf('/');
        if (index > 0) {
            QByteArray dir = k.left(index);
            gconf_client_add_dir(client, dir.data(), GCONF_CLIENT_PRELOAD_ONELEVEL, &error);
        } else {
            gconf_client_add_dir(client, k.data(), GCONF_CLIENT_PRELOAD_NONE, &error);
        }

        if(error) {
            qWarning() << "MImSettingsGConfBackend" << error->message;
            g_error_free(error);
            return;
        }
        d->notify_id = gconf_client_notify_add(client, k.data(),
                       MImSettingsGConfBackendPrivate::notify_trampoline, this,
                       NULL, &error);
        if(error) {
            qWarning() << "MImSettingsGConfBackend" << error->message;
            g_error_free(error);
            d->have_gconf = false;
            return;
        }
        update_value(false);
    }
    d->have_gconf = true;
}

MImSettingsGConfBackend::~MImSettingsGConfBackend()
{
    Q_D(MImSettingsGConfBackend);

    if(d->have_gconf) {
        withClient(client) {
            QByteArray k = convertKey(d->key);
            gconf_client_notify_remove(client, d->notify_id);
            GError *error = NULL;

            // Use the same dir as in ctor
            int index = k.lastIndexOf('/');
            if (index > 0) {
                k = k.left(index);
            }
            gconf_client_remove_dir(client, k.data(), &error);

            if(error) {
                qWarning() << "MImSettingsGConfBackend" << error->message;
                g_error_free(error);
            }
        }
    }
}


MImSettingsBackend *MImSettingsGConfBackendFactory::create(const QString &key, QObject *parent)
{
    return new MImSettingsGConfBackend(key, parent);
}
