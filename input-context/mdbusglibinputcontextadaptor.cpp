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
#include "minputcontext.h"
#include <minputmethodnamespace.h>


G_DEFINE_TYPE(MDBusGlibInputContextAdaptor, m_dbus_glib_input_context_adaptor, G_TYPE_OBJECT)


static gboolean m_dbus_glib_input_context_adaptor_activation_lost_event(
    MDBusGlibInputContextAdaptor *obj, GError **/*error*/)
{
    obj->inputContext->activationLostEvent();
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_im_initiated_hide(
    MDBusGlibInputContextAdaptor *obj, GError **/*error*/)
{
    obj->inputContext->imInitiatedHide();
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_commit_string(
    MDBusGlibInputContextAdaptor *obj, const char *string, gint32 replaceStart,
    gint32 replaceLength, gint32 cursorPos, GError **/*error*/)
{
    obj->inputContext->commitString(QString::fromUtf8(string), replaceStart,
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
                                                                 GArray *preeditFormats,
                                                                 gint32 replaceStart,
                                                                 gint32 replaceLength,
                                                                 gint32 cursorPos, GError **/*error*/)
{
    const QByteArray storageWrapper(QByteArray::fromRawData(preeditFormats->data, preeditFormats->len));
    QDataStream formatListStream(storageWrapper);
    QList<MInputMethod::PreeditTextFormat> formatList;
    formatListStream >> formatList;
    if (formatListStream.status() == QDataStream::Ok) {
        obj->inputContext->updatePreedit(QString::fromUtf8(string),
                                         formatList,
                                         replaceStart,
                                         replaceLength,
                                         cursorPos);
    }
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_key_event(
    MDBusGlibInputContextAdaptor *obj, gint32 type, gint32 key, gint32 modifiers, const char *text,
    gboolean autoRepeat, gint32 count, guchar requestType, GError **/*error*/)
{
    obj->inputContext->keyEvent(type, key, modifiers, QString::fromUtf8(text),
                                autoRepeat == TRUE, count,
                                static_cast<MInputMethod::EventRequestType>(requestType));
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_update_input_method_area(
    MDBusGlibInputContextAdaptor *obj, GArray *rectList, GError **/*error*/)
{
    const QByteArray storageWrapper(QByteArray::fromRawData(rectList->data, rectList->len));
    QDataStream rectListStream(storageWrapper);
    QList<QVariant> deserializedRectList;
    rectListStream >> deserializedRectList;
    if (rectListStream.status() == QDataStream::Ok) {
        obj->inputContext->updateInputMethodArea(deserializedRectList);
    } else {
        qWarning("MInputContext: Invalid parameter to updateInputMethodArea.");
    }
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_set_global_correction_enabled(
    MDBusGlibInputContextAdaptor *obj, gboolean value, GError **/*error*/)
{
    obj->inputContext->setGlobalCorrectionEnabled(value == TRUE);
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_preedit_rectangle(
    MDBusGlibInputContextAdaptor *obj, gboolean *valid, gint *x, gint *y,
    gint *width, gint *height, GError **/*error*/)
{
    bool deserializedValidity;
    const QRect rect(obj->inputContext->preeditRectangle(deserializedValidity));
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
    obj->inputContext->copy();
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_paste(
    MDBusGlibInputContextAdaptor *obj, GError **/*error*/)
{
    obj->inputContext->paste();
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_set_redirect_keys(
    MDBusGlibInputContextAdaptor *obj, gboolean enabled, GError **/*error*/)
{
    obj->inputContext->setRedirectKeys(enabled == TRUE);
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_set_detectable_auto_repeat(
    MDBusGlibInputContextAdaptor *obj, gboolean enabled, GError **/*error*/)
{
    obj->inputContext->setDetectableAutoRepeat(enabled == TRUE);
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_set_selection(MDBusGlibInputContextAdaptor *obj,
                                                                gint32 start, gint32 length, GError **/*error*/)
{
    obj->inputContext->setSelection(start,length);
    return TRUE;
}

static gboolean m_dbus_glib_input_context_adaptor_selection(
    MDBusGlibInputContextAdaptor *obj, gboolean *valid, gchar **gdata, GError **/*error*/)
{
    bool validity;
    const QString selection(obj->inputContext->selection(validity));
    *valid = validity ? TRUE : FALSE;
    *gdata = NULL;

    if (validity && !selection.isEmpty()) {
        *gdata = g_strdup(selection.toUtf8().data());
    }
    return TRUE;
}

#include "mdbusglibicconnectionserviceglue.h"


static void
m_dbus_glib_input_context_adaptor_init(MDBusGlibInputContextAdaptor */*obj*/)
{
}

static void
m_dbus_glib_input_context_adaptor_class_init(MDBusGlibInputContextAdaptorClass */*klass*/)
{
    dbus_g_object_type_install_info(M_TYPE_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR, &dbus_glib_m_dbus_glib_input_context_adaptor_object_info);
}
