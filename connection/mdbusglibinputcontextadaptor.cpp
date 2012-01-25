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

#include "mdbusglibinputcontextadaptor.h"
#include "mimserverconnection.h"

#include <minputmethodnamespace.h>
#include <variantmarshalling.h>

#include <dbus/dbus.h>

G_DEFINE_TYPE(MDBusGlibInputContextAdaptor, m_dbus_glib_input_context_adaptor, G_TYPE_OBJECT)


static gboolean m_dbus_glib_input_context_adaptor_activation_lost_event(
    MDBusGlibInputContextAdaptor *obj, GError **/*error*/)
{
    Q_EMIT obj->imServerConnection->activationLostEvent();
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_im_initiated_hide(
    MDBusGlibInputContextAdaptor *obj, GError **/*error*/)
{
    Q_EMIT obj->imServerConnection->imInitiatedHide();
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_commit_string(
    MDBusGlibInputContextAdaptor *obj, const char *string, gint32 replaceStart,
    gint32 replaceLength, gint32 cursorPos, GError **/*error*/)
{
    Q_EMIT obj->imServerConnection->commitString(QString::fromUtf8(string), replaceStart,
                                    replaceLength, cursorPos);
    return TRUE;
}

QDataStream &operator>>(QDataStream &s, MInputMethod::PreeditTextFormat &t)
{
    int preeditFace;
    s >> t.start;
    s >> t.length;
    s >> preeditFace;
    t.preeditFace = static_cast<MInputMethod::PreeditFace>(preeditFace);
    return s;
}

static gboolean m_dbus_glib_input_context_adaptor_update_preedit(MDBusGlibInputContextAdaptor *obj,
                                                                 const char *string,
                                                                 GPtrArray *formatListData,
                                                                 gint32 replaceStart,
                                                                 gint32 replaceLength,
                                                                 gint32 cursorPos, GError **/*error*/)
{
    QList<MInputMethod::PreeditTextFormat> formatList;
    for (guint i = 0; i < formatListData->len; ++i) {
        GValueArray *itemData = ((GValueArray**)formatListData->pdata)[i];
        formatList.push_back(MInputMethod::PreeditTextFormat(
                                    g_value_get_int(g_value_array_get_nth(itemData, 0)),
                                    g_value_get_int(g_value_array_get_nth(itemData, 1)),
                                    MInputMethod::PreeditFace(
                                        g_value_get_int(g_value_array_get_nth(itemData, 2)))));
    }
    Q_EMIT obj->imServerConnection->updatePreedit(QString::fromUtf8(string),
                                     formatList,
                                     replaceStart,
                                     replaceLength,
                                     cursorPos);
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_key_event(
    MDBusGlibInputContextAdaptor *obj, gint32 type, gint32 key, gint32 modifiers, const char *text,
    gboolean autoRepeat, gint32 count, guchar requestType, GError **/*error*/)
{
    Q_EMIT obj->imServerConnection->keyEvent(type, key, modifiers, QString::fromUtf8(text),
                                autoRepeat == TRUE, count,
                                static_cast<MInputMethod::EventRequestType>(requestType));
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_update_input_method_area(
    MDBusGlibInputContextAdaptor *obj,
    gint32 left, gint32 top, gint32 width, gint32 height,
    GError **/*error*/)
{
    Q_EMIT obj->imServerConnection->updateInputMethodArea(QRect(left, top, width, height));
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_set_global_correction_enabled(
    MDBusGlibInputContextAdaptor *obj, gboolean value, GError **/*error*/)
{
    Q_EMIT obj->imServerConnection->setGlobalCorrectionEnabled(value == TRUE);
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_preedit_rectangle(
    MDBusGlibInputContextAdaptor *obj, gboolean *valid, gint *x, gint *y,
    gint *width, gint *height, GError **/*error*/)
{
    bool deserializedValidity;
    QRect rect;
    Q_EMIT obj->imServerConnection->getPreeditRectangle(rect, deserializedValidity);
    *valid = deserializedValidity ? TRUE : FALSE;
    *x = rect.x();
    *y = rect.y();
    *width = rect.width();
    *height = rect.height();
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_copy(
    MDBusGlibInputContextAdaptor *obj, GError **/*error*/)
{
    Q_EMIT obj->imServerConnection->copy();
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_paste(
    MDBusGlibInputContextAdaptor *obj, GError **/*error*/)
{
    Q_EMIT obj->imServerConnection->paste();
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_set_redirect_keys(
    MDBusGlibInputContextAdaptor *obj, gboolean enabled, GError **/*error*/)
{
    Q_EMIT obj->imServerConnection->setRedirectKeys(enabled == TRUE);
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_set_detectable_auto_repeat(
    MDBusGlibInputContextAdaptor *obj, gboolean enabled, GError **/*error*/)
{
    Q_EMIT obj->imServerConnection->setDetectableAutoRepeat(enabled == TRUE);
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_set_selection(MDBusGlibInputContextAdaptor *obj,
                                                                gint32 start, gint32 length, GError **/*error*/)
{
    Q_EMIT obj->imServerConnection->setSelection(start,length);
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_selection(
    MDBusGlibInputContextAdaptor *obj, gboolean *valid, gchar **gdata, GError **/*error*/)
{
    bool validity;
    QString selection;
    Q_EMIT obj->imServerConnection->getSelection(selection, validity);
    *valid = validity ? TRUE : FALSE;
    *gdata = NULL;

    if (validity && !selection.isEmpty()) {
        *gdata = g_strdup(selection.toUtf8().data());
    }
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_set_language(
    MDBusGlibInputContextAdaptor *obj, const char *string,
    GError **/*error*/)
{
    Q_EMIT obj->imServerConnection->setLanguage(QString::fromUtf8(string));
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_notify_extended_attribute_changed(
        MDBusGlibInputContextAdaptor *obj,
        gint32 id,
        const char *target,
        const char *targetItem,
        const char *attribute,
        GValue *valueData,
        GError **/*error*/)
{
    QVariant value;
    QString error_message;
    if (decodeVariant(&value, valueData, &error_message)) {
         Q_EMIT obj->imServerConnection->extendedAttributeChanged(static_cast<int>(id),
                                                                  QString::fromUtf8(target),
                                                                  QString::fromUtf8(targetItem),
                                                                  QString::fromUtf8(attribute),
                                                                  value);
    } else {
        qWarning() << "notify_extended_attribute_changed.arg[4]" + error_message;
    }
    return TRUE;
}

#include "mdbusglibinputcontextadaptorglue.h"


static void
m_dbus_glib_input_context_adaptor_init(MDBusGlibInputContextAdaptor */*obj*/)
{
}

static void
m_dbus_glib_input_context_adaptor_class_init(MDBusGlibInputContextAdaptorClass */*klass*/)
{
    dbus_g_object_type_install_info(M_TYPE_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR, &dbus_glib_m_dbus_glib_input_context_adaptor_object_info);
}
