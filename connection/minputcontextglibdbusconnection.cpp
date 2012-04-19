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

#define _BSD_SOURCE             // for mkdtemp

#include "minputcontextglibdbusconnection.h"
#include "serverdbusaddress.h"
#include <variantmarshalling.h>
#include <maliit/settingdata.h>

#include <QDataStream>
#include <QDebug>
#include <QRegion>
#include <QKeyEvent>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QFile>
#include <QDir>

#include <dbus/dbus.h>

#include <stdint.h> 

namespace
{
    const char * const DBusPath = "/com/meego/inputmethod/uiserver1";

    const char * const DBusClientPath = "/com/meego/inputmethod/inputcontext";
    const char * const DBusClientInterface = "com.meego.inputmethod.inputcontext1";

    bool encodePreeditFormats(GType *typeDest, GPtrArray **dataDest, const QList<Maliit::PreeditTextFormat> &preeditFormats)
    {
        // dbus datatype is a(iii)

        // type is staightforward, only selection of constructors is not obvious
        GType itemType = dbus_g_type_get_struct("GValueArray",
                                                G_TYPE_INT, G_TYPE_INT, G_TYPE_INT,
                                                G_TYPE_INVALID);
        *typeDest = dbus_g_type_get_collection("GPtrArray", itemType);

        // representation is:
        //  QList<PTF>, "a(iii)", GPtrArray with elements
        //   PTF, "iii", GValueArray with elements
        //    PTF::start, "i", GValue with type int
        //    PTF::length, "i", GValue with type int
        //    PTF::preeditFace, "i", GValue with type int
        // see also decoding in m_dbus_glib_input_context_adaptor_update_preedit(),
        // though it is much shorter
        *dataDest = (GPtrArray*)dbus_g_type_specialized_construct(*typeDest);
        if (*dataDest == 0) {
            qWarning() << Q_FUNC_INFO << "failed to initalize PreeditTextFormat container";
            return false;
        }
        Q_FOREACH (Maliit::PreeditTextFormat formatItem, preeditFormats) {
            GValueArray *itemData = (GValueArray*)dbus_g_type_specialized_construct(itemType);
            if (itemData == 0) {
                qWarning() << Q_FUNC_INFO << "failed to initalize PreeditTextFormat item";
                dbus_g_type_collection_peek_vtable(*typeDest)->base_vtable.free_func(*typeDest, *dataDest);
                return false;
            }
            GValue itemDataWrap =  {0, {{0}, {0}}};
            g_value_init(&itemDataWrap, itemType);
            g_value_set_static_boxed(&itemDataWrap, itemData);
            if (!dbus_g_type_struct_set(&itemDataWrap,
                                        0, formatItem.start,
                                        1, formatItem.length,
                                        2, formatItem.preeditFace,
                                        G_MAXUINT)) {
                qWarning() << Q_FUNC_INFO << "failed to fill PreeditTextFormat item";
                g_value_array_free(itemData);
                g_value_unset(&itemDataWrap);
                dbus_g_type_collection_peek_vtable(*typeDest)->base_vtable.free_func(*typeDest, *dataDest);
                return false;
            }
            g_value_reset(&itemDataWrap);
            g_ptr_array_add(*dataDest, itemData);
        }
        return true;
    }

    bool encodeSettings(GType *typeDest, GPtrArray **dataDest, const QList<MImPluginSettingsInfo> &settings)
    {
        GType attrMapType = dbus_g_type_get_map("GHashTable", G_TYPE_STRING, G_TYPE_VALUE);
        GType entryType = dbus_g_type_get_struct("GValueArray",
                                                 G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT,
                                                 G_TYPE_BOOLEAN, G_TYPE_VALUE, attrMapType,
                                                 G_TYPE_INVALID);
        GType entryArrayType = dbus_g_type_get_collection("GPtrArray", entryType);
        GType pluginType = dbus_g_type_get_struct("GValueArray",
                                                  G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, entryArrayType,
                                                  G_TYPE_INVALID);
        *typeDest = dbus_g_type_get_collection("GPtrArray", pluginType);

        *dataDest = (GPtrArray*)dbus_g_type_specialized_construct(*typeDest);

        if (*dataDest == 0) {
            qWarning() << Q_FUNC_INFO << "failed to initalize MImPluginSettingsInfo container";
            return false;
        }
        Q_FOREACH (const MImPluginSettingsInfo &plugin, settings) {
            GPtrArray *entryArray = (GPtrArray*)dbus_g_type_specialized_construct(entryArrayType);
            if (entryArray == 0) {
                qWarning() << Q_FUNC_INFO << "failed to initalize MImPluginSettingsEntry container";
                dbus_g_type_collection_peek_vtable(*typeDest)->base_vtable.free_func(*typeDest, *dataDest);
                return false;
            }

            Q_FOREACH (const MImPluginSettingsEntry &entry, plugin.entries) {
                GHashTable *attributes = encodeVariantMap(entry.attributes);
                GValueArray *entryData = (GValueArray*)dbus_g_type_specialized_construct(entryType);

                if (entryData == 0) {
                    qWarning() << Q_FUNC_INFO << "failed to initalize MImPluginSettingsEntry item";
                    dbus_g_type_collection_peek_vtable(entryArrayType)->base_vtable.free_func(entryArrayType, entryArray);
                    dbus_g_type_collection_peek_vtable(*typeDest)->base_vtable.free_func(*typeDest, *dataDest);
                    return false;
                }
                GValue *current_value = g_new0(GValue, 1);
                if (!entry.value.isValid()) {
                    // any type/value will do, validity is encoded in a separate boolean
                    g_value_init(current_value, G_TYPE_INT);
                } else if (!encodeVariant(current_value, entry.value)) {
                    g_free(current_value);
                    dbus_g_type_collection_peek_vtable(entryType)->base_vtable.free_func(entryType, entryData);
                    dbus_g_type_collection_peek_vtable(entryArrayType)->base_vtable.free_func(entryArrayType, entryArray);
                    dbus_g_type_collection_peek_vtable(*typeDest)->base_vtable.free_func(*typeDest, *dataDest);
                    return 0;
                }
                GValue entryDataWrap =  {0, {{0}, {0}}};
                g_value_init(&entryDataWrap, entryType);
                g_value_set_static_boxed(&entryDataWrap, entryData);
                if (!dbus_g_type_struct_set(&entryDataWrap,
                                            0, entry.description.toUtf8().data(),
                                            1, entry.extension_key.toUtf8().data(),
                                            2, entry.type,
                                            3, entry.value.isValid(),
                                            4, current_value,
                                            5, attributes,
                                            G_MAXUINT)) {
                    qWarning() << Q_FUNC_INFO << "failed to fill MImPluginSettingsEntry item";
                    g_value_array_free(entryData);
                    g_value_unset(&entryDataWrap);
                    dbus_g_type_collection_peek_vtable(entryType)->base_vtable.free_func(entryType, entryData);
                    dbus_g_type_collection_peek_vtable(entryArrayType)->base_vtable.free_func(entryArrayType, entryArray);
                    dbus_g_type_collection_peek_vtable(*typeDest)->base_vtable.free_func(*typeDest, *dataDest);
                    return false;
                }
                g_value_reset(&entryDataWrap);
                g_ptr_array_add(entryArray, entryData);
            }

            GValueArray *pluginData = (GValueArray*)dbus_g_type_specialized_construct(pluginType);
            if (pluginData == 0) {
                qWarning() << Q_FUNC_INFO << "failed to initalize MImPluginSettingsInfo item";
                dbus_g_type_collection_peek_vtable(entryArrayType)->base_vtable.free_func(entryArrayType, entryArray);
                dbus_g_type_collection_peek_vtable(*typeDest)->base_vtable.free_func(*typeDest, *dataDest);
                return false;
            }
            GValue pluginDataWrap =  {0, {{0}, {0}}};
            g_value_init(&pluginDataWrap, pluginType);
            g_value_set_static_boxed(&pluginDataWrap, pluginData);
            if (!dbus_g_type_struct_set(&pluginDataWrap,
                                        0, plugin.description_language.toUtf8().data(),
                                        1, plugin.plugin_name.toUtf8().data(),
                                        2, plugin.plugin_description.toUtf8().data(),
                                        3, plugin.extension_id,
                                        4, entryArray,
                                        G_MAXUINT)) {
                qWarning() << Q_FUNC_INFO << "failed to fill MImPluginSettingsInfo item";
                g_value_array_free(pluginData);
                g_value_unset(&pluginDataWrap);
                dbus_g_type_collection_peek_vtable(pluginType)->base_vtable.free_func(pluginType, pluginData);
                dbus_g_type_collection_peek_vtable(entryArrayType)->base_vtable.free_func(entryArrayType, entryArray);
                dbus_g_type_collection_peek_vtable(*typeDest)->base_vtable.free_func(*typeDest, *dataDest);
                return false;
            }
            g_value_reset(&pluginDataWrap);
            g_ptr_array_add(*dataDest, pluginData);
        }

        return true;
    }
}


//! \internal
//! \brief GObject-based input context connection class
//!
//! There is one of these for each client connection
struct MDBusGlibICConnection
{
    GObject parent;

    //! This is the glib-level connection established when the input context calls us
    DBusGConnection* dbusConnection;
    //! This is a proxy object used to call input context methods
    DBusGProxy *inputContextProxy;
    //! The actual C++-level connection that acts as a proxy for the currently active input context
    MInputContextGlibDBusConnection *icConnection;
    //! Running number used to identify the connection, used for toolbar ids
    unsigned int connectionNumber;
};

struct MDBusGlibICConnectionClass
{
    GObjectClass parent;
};
//! \internal_end

#define M_TYPE_DBUS_GLIB_IC_CONNECTION              (m_dbus_glib_ic_connection_get_type())
#define M_DBUS_GLIB_IC_CONNECTION(object)           (G_TYPE_CHECK_INSTANCE_CAST((object), M_TYPE_DBUS_GLIB_IC_CONNECTION, MDBusGlibICConnection))
#define M_DBUS_GLIB_IC_CONNECTION_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), M_TYPE_DBUS_GLIB_IC_CONNECTION, MDBusGlibICConnectionClass))
#define M_IS_M_DBUS_GLIB_IC_CONNECTION(object)      (G_TYPE_CHECK_INSTANCE_TYPE((object), M_TYPE_DBUS_GLIB_IC_CONNECTION))
#define M_IS_M_DBUS_GLIB_IC_CONNECTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), M_TYPE_DBUS_GLIB_IC_CONNECTION))
#define M_DBUS_GLIB_IC_CONNECTION_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), M_TYPE_DBUS_GLIB_IC_CONNECTION, MDBusGlibICConnectionClass))

G_DEFINE_TYPE(MDBusGlibICConnection, m_dbus_glib_ic_connection, G_TYPE_OBJECT)

static gboolean
m_dbus_glib_ic_connection_activate_context(MDBusGlibICConnection *obj, GError **/*error*/)
{
    obj->icConnection->activateContext(obj->connectionNumber);
    return TRUE;
}

static gboolean
m_dbus_glib_ic_connection_show_input_method(MDBusGlibICConnection *obj, GError **/*error*/)
{
    obj->icConnection->showInputMethod(obj->connectionNumber);
    return TRUE;
}

static gboolean
m_dbus_glib_ic_connection_hide_input_method(MDBusGlibICConnection *obj, GError **/*error*/)
{
    obj->icConnection->hideInputMethod(obj->connectionNumber);
    return TRUE;
}

static gboolean
m_dbus_glib_ic_connection_mouse_clicked_on_preedit(MDBusGlibICConnection *obj,
                                                   gint32 posX, gint32 posY,
                                                   gint32 preeditX, gint32 preeditY,
                                                   gint32 preeditWidth, gint32 preeditHeight,
                                                   GError **/*error*/)
{
    obj->icConnection->mouseClickedOnPreedit(obj->connectionNumber, QPoint(posX, posY), QRect(preeditX, preeditY,
                                                                            preeditWidth, preeditHeight));
    return TRUE;
}

static gboolean
m_dbus_glib_ic_connection_set_preedit(MDBusGlibICConnection *obj, const char *text,
                                      gint32 cursorPos, GError **/*error*/)
{
    obj->icConnection->setPreedit(obj->connectionNumber, QString::fromUtf8(text), cursorPos);
    return TRUE;
}


static gboolean
m_dbus_glib_ic_connection_update_widget_information(MDBusGlibICConnection *obj,
                                                    GHashTable *stateInformation,
                                                    gboolean focusChanged, GError **/*error*/)
{
    QMap<QString, QVariant> stateMap;
    QString error_message;
    if (decodeVariantMap(&stateMap, stateInformation, &error_message)) {
        obj->icConnection->updateWidgetInformation(obj->connectionNumber, stateMap, focusChanged == TRUE);
    } else {
        qWarning() << "updateWidgetInformation.arg[0]" + error_message;
    }
    return TRUE;
}

static gboolean
m_dbus_glib_ic_connection_reset(MDBusGlibICConnection *obj, GError **/*error*/)
{
    obj->icConnection->reset(obj->connectionNumber);
    return TRUE;
}

static gboolean
m_dbus_glib_ic_connection_app_orientation_about_to_change(MDBusGlibICConnection *obj, gint32 angle,
                                                         GError **/*error*/)
{
    obj->icConnection->receivedAppOrientationAboutToChange(obj->connectionNumber, static_cast<int>(angle));
    return TRUE;
}


static gboolean
m_dbus_glib_ic_connection_app_orientation_changed(MDBusGlibICConnection *obj, gint32 angle,
                                                  GError **/*error*/)
{
    obj->icConnection->receivedAppOrientationChanged(obj->connectionNumber, static_cast<int>(angle));
    return TRUE;
}

static gboolean
m_dbus_glib_ic_connection_set_copy_paste_state(MDBusGlibICConnection *obj, gboolean copyAvailable,
                                               gboolean pasteAvailable, GError **/*error*/)
{
    obj->icConnection->setCopyPasteState(obj->connectionNumber, copyAvailable == TRUE, pasteAvailable == TRUE);
    return TRUE;
}

static gboolean
m_dbus_glib_ic_connection_process_key_event(MDBusGlibICConnection *obj, gint32 keyType,
                                            gint32 keyCode, gint32 modifiers,
                                            const char *text, gboolean autoRepeat, gint32 count,
                                            guint32 nativeScanCode, guint32 nativeModifiers,
                                            unsigned long time,
                                            GError **/*error*/)
{
    obj->icConnection->processKeyEvent(obj->connectionNumber, static_cast<QEvent::Type>(keyType),
                                       static_cast<Qt::Key>(keyCode),
                                       static_cast<Qt::KeyboardModifiers>(modifiers),
                                       QString::fromUtf8(text), autoRepeat == TRUE,
                                       static_cast<int>(count), static_cast<quint32>(nativeScanCode),
                                       static_cast<quint32>(nativeModifiers), time);
    return TRUE;
}

static gboolean
m_dbus_glib_ic_connection_register_attribute_extension(MDBusGlibICConnection *obj, gint32 id,
                                                       const char *fileName, GError **/*error*/)
{
    obj->icConnection->registerAttributeExtension(obj->connectionNumber, static_cast<int>(id), QString::fromUtf8(fileName));
    return TRUE;
}

static gboolean
m_dbus_glib_ic_connection_unregister_attribute_extension(MDBusGlibICConnection *obj, gint32 id,
                                                         GError **/*error*/)
{
    obj->icConnection->unregisterAttributeExtension(obj->connectionNumber, static_cast<int>(id));
    return TRUE;
}

static gboolean
m_dbus_glib_ic_connection_set_extended_attribute(MDBusGlibICConnection *obj, gint32 id,
                                                 const char *target, const char *targetItem,
                                                 const char *attribute, GValue *valueData, GError **/*error*/)
{
    qDebug() << __PRETTY_FUNCTION__;
    QVariant value;
    QString error_message;
    if (decodeVariant(&value, valueData, &error_message)) {
        obj->icConnection->setExtendedAttribute(obj->connectionNumber, static_cast<int>(id), QString::fromUtf8(target),
                                                QString::fromUtf8(targetItem),
                                                QString::fromUtf8(attribute),
                                                value);
    } else {
        qWarning() << "setExtendedAttribute.arg[4]" + error_message;
    }
    return TRUE;
}

static gboolean
m_dbus_glib_ic_connection_load_plugin_settings(MDBusGlibICConnection *obj, const char *descriptionLanguage,
                                               GError **/*error*/)
{
    obj->icConnection->loadPluginSettings(obj->connectionNumber, QString::fromUtf8(descriptionLanguage));
    return TRUE;
}


#include "mdbusglibicconnectionserviceglue.h"

static void
m_dbus_glib_ic_connection_dispose(GObject *object)
{
    MDBusGlibICConnection *self(M_DBUS_GLIB_IC_CONNECTION(object));

    if (self->dbusConnection) {
        dbus_g_connection_unref(self->dbusConnection);
        self->dbusConnection = 0;
    }

    G_OBJECT_CLASS(m_dbus_glib_ic_connection_parent_class)->dispose(object);
}


static void
m_dbus_glib_ic_connection_finalize(GObject *object)
{
    G_OBJECT_CLASS(m_dbus_glib_ic_connection_parent_class)->finalize(object);
}


static void
m_dbus_glib_ic_connection_init(MDBusGlibICConnection */*obj*/)
{
}

static void
m_dbus_glib_ic_connection_class_init(MDBusGlibICConnectionClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose = m_dbus_glib_ic_connection_dispose;
    gobject_class->finalize = m_dbus_glib_ic_connection_finalize;

    dbus_g_object_type_install_info(M_TYPE_DBUS_GLIB_IC_CONNECTION,
                                    &dbus_glib_m_dbus_glib_ic_connection_object_info);
}


// MInputContextGlibDBusConnection...........................................

static void handleDisconnectionTrampoline(DBusGProxy */*proxy*/, gpointer userData)
{
    qDebug() << __PRETTY_FUNCTION__;

    MDBusGlibICConnection *connection(M_DBUS_GLIB_IC_CONNECTION(userData));
    connection->icConnection->handleDisconnection(connection->connectionNumber);
}

void MInputContextGlibDBusConnection::handleDisconnection(unsigned int connectionId)
{
    MInputContextConnection::handleDisconnection(connectionId);

    MDBusGlibICConnection *connection = connectionObj(connectionId);
    if (connection) {
        g_object_unref(G_OBJECT(connection));
        mConnections.remove(connectionId);
    }
}

void MInputContextGlibDBusConnection::handleNewDBusConnectionReady(MDBusGlibICConnection *connectionObj)
{
    // For newly connected applications, input language should be sent because
    // applications may need the information even before plugin is actually showed.
    setLanguage(connectionObj, lastLanguage);
}

static void handleNewConnectionTrampoline(DBusServer */*server*/, DBusConnection *connection, gpointer userData)
{
    MInputContextGlibDBusConnection *icConnection = static_cast<MInputContextGlibDBusConnection *>(userData);

    dbus_connection_ref(connection);
    dbus_connection_setup_with_g_main(connection, NULL);

    MDBusGlibICConnection *obj = M_DBUS_GLIB_IC_CONNECTION(
        g_object_new(M_TYPE_DBUS_GLIB_IC_CONNECTION, NULL));

    obj->dbusConnection = dbus_connection_get_g_connection(connection);
    obj->icConnection = icConnection;

    icConnection->handleNewConnection(obj);
}

void MInputContextGlibDBusConnection::handleNewConnection(MDBusGlibICConnection *obj)
{
    qDebug() << __PRETTY_FUNCTION__;

    DBusConnection *connection = dbus_g_connection_get_connection(obj->dbusConnection);
    dbus_connection_set_allow_anonymous(connection, mAllowAnonymous);

    // Proxy for calling input context methods
    DBusGProxy *inputContextProxy = dbus_g_proxy_new_for_peer(
        obj->dbusConnection, DBusClientPath, DBusClientInterface);
    if (!inputContextProxy) {
        qFatal("Unable to find the service.");
    }
    obj->inputContextProxy = inputContextProxy;

    g_signal_connect(G_OBJECT(inputContextProxy), "destroy",
                     G_CALLBACK(handleDisconnectionTrampoline), obj);

    static unsigned int connectionCounter = 1; // Start at 1 so 0 can be used as a sentinel value
    obj->connectionNumber = connectionCounter++;

    insertNewConnection(obj->connectionNumber, obj);

    dbus_g_connection_register_g_object(obj->dbusConnection, DBusPath, G_OBJECT(obj));

    handleNewDBusConnectionReady(obj);
}

void MInputContextGlibDBusConnection::insertNewConnection(unsigned int connectionId,
                                                          MDBusGlibICConnection *connectionObj)
{
    mConnections.insert(connectionId, connectionObj);
}

MInputContextGlibDBusConnection::MInputContextGlibDBusConnection(std::tr1::shared_ptr<Maliit::Server::DBus::Address> address, bool allowAnonymous)
  : mAddress(address)
  , mAllowAnonymous(allowAnonymous)
  , server(0)
{
    dbus_g_thread_init();
    g_type_init();

    server = mAddress->connect();

    dbus_server_setup_with_g_main(server, NULL);
    dbus_server_set_new_connection_function(server, handleNewConnectionTrampoline, this, NULL);
}

MInputContextGlibDBusConnection::~MInputContextGlibDBusConnection()
{
    dbus_server_disconnect(server);
    dbus_server_unref(server);
}

MDBusGlibICConnection *MInputContextGlibDBusConnection::activeContext()
{
    return connectionObj(activeConnection);
}
MDBusGlibICConnection *MInputContextGlibDBusConnection::connectionObj(unsigned int connectionId)
{
    if (!connectionId) {
        return 0;
    }

    MDBusGlibICConnection *context = mConnections.value(connectionId, 0);
    if (!context) {
        qCritical() << "Could not find connection object for connection ID " << connectionId;
    }
    return context;
}

// Server -> input context...................................................

QDataStream &operator<<(QDataStream &s, const Maliit::PreeditTextFormat &t)
{
    s << (qint32)t.start << (qint32)t.length
      << (qint32)t.preeditFace;
    return s;
}

void MInputContextGlibDBusConnection::sendPreeditString(const QString &string,
                                                        const QList<Maliit::PreeditTextFormat> &preeditFormats,
                                                        int replaceStart, int replaceLength,
                                                        int cursorPos)
{
    if (activeConnection) {
        MInputContextConnection::sendPreeditString(string, preeditFormats,
                                                   replaceStart, replaceLength, cursorPos);

        GType preeditFormatsType;
        GPtrArray *preeditFormatsData;
        if (!encodePreeditFormats(&preeditFormatsType, &preeditFormatsData, preeditFormats)) {
            return;
        }

        dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "updatePreedit",
                                   G_TYPE_STRING, string.toUtf8().data(),
                                   preeditFormatsType, preeditFormatsData,
                                   G_TYPE_INT, replaceStart,
                                   G_TYPE_INT, replaceLength,
                                   G_TYPE_INT, cursorPos,
                                   G_TYPE_INVALID);
        dbus_g_type_collection_peek_vtable(preeditFormatsType)->base_vtable.free_func(preeditFormatsType, preeditFormatsData);
    }
}


void MInputContextGlibDBusConnection::sendCommitString(const QString &string, int replaceStart,
                                                       int replaceLength, int cursorPos)
{
    if (activeConnection) {

        MInputContextConnection::sendCommitString(string, replaceStart, replaceLength, cursorPos);

        dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "commitString",
                                   G_TYPE_STRING, string.toUtf8().data(),
                                   G_TYPE_INT, replaceStart,
                                   G_TYPE_INT, replaceLength,
                                   G_TYPE_INT, cursorPos,
                                   G_TYPE_INVALID);
    }
}


void MInputContextGlibDBusConnection::sendKeyEvent(const QKeyEvent &keyEvent,
                                                   Maliit::EventRequestType requestType)
{
    if (activeConnection) {
        MInputContextConnection::sendKeyEvent(keyEvent, requestType);

        int type = static_cast<int>(keyEvent.type());
        int key = static_cast<int>(keyEvent.key());
        int modifiers = static_cast<int>(keyEvent.modifiers());

        dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "keyEvent",
                                   G_TYPE_INT, type,
                                   G_TYPE_INT, key,
                                   G_TYPE_INT, modifiers,
                                   G_TYPE_STRING, keyEvent.text().toUtf8().data(),
                                   G_TYPE_BOOLEAN, keyEvent.isAutoRepeat(),
                                   G_TYPE_INT, keyEvent.count(),
                                   G_TYPE_UCHAR, static_cast<uchar>(requestType),
                                   G_TYPE_INVALID);
    }
}


void MInputContextGlibDBusConnection::notifyImInitiatedHiding()
{
    if (activeContext()) {
        dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "imInitiatedHide",
                                   G_TYPE_INVALID);
    }
}



void MInputContextGlibDBusConnection::setGlobalCorrectionEnabled(bool enabled)
{
    if ((enabled != globalCorrectionEnabled()) && activeContext()) {
        dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "setGlobalCorrectionEnabled",
                                   G_TYPE_BOOLEAN, enabled,
                                   G_TYPE_INVALID);

        MInputContextConnection::setGlobalCorrectionEnabled(enabled);
    }
}

QString MInputContextGlibDBusConnection::selection(bool &valid)
{
    GError *error = NULL;

    QString selectionText;
    gboolean gvalidity = FALSE;
    gchar *gdata = NULL;
    if (activeContext() && dbus_g_proxy_call(activeContext()->inputContextProxy, "selection", &error,
                                           G_TYPE_INVALID,
                                           G_TYPE_BOOLEAN, &gvalidity,
                                           G_TYPE_STRING, &gdata,
                                           G_TYPE_INVALID)) {
        if (gdata) {
            selectionText = QString::fromUtf8(gdata);
            g_free(gdata);
        }
        valid = gvalidity == TRUE;
    } else {
        if (error) { // dbus_g_proxy_call may return FALSE and not set error despite what the doc says
            g_error_free(error);
        }
        valid = false;
        return QString();        
    }

    return selectionText;
}

void MInputContextGlibDBusConnection::setLanguage(const QString &language)
{
    lastLanguage = language;
    setLanguage(activeContext(), language);
}

void MInputContextGlibDBusConnection::setLanguage(MDBusGlibICConnection *targetIcConnection,
                                                  const QString &language)
{
    if (targetIcConnection) {
        dbus_g_proxy_call_no_reply(targetIcConnection->inputContextProxy, "setLanguage",
                                   G_TYPE_STRING, language.toUtf8().data(),
                                   G_TYPE_INVALID);
    }
}

QRect MInputContextGlibDBusConnection::preeditRectangle(bool &valid)
{
    GError *error = NULL;

    gboolean gvalidity;
    gint32 x, y, width, height;

    if (activeContext() &&
        dbus_g_proxy_call(activeContext()->inputContextProxy, "preeditRectangle", &error, G_TYPE_INVALID,
                          G_TYPE_BOOLEAN, &gvalidity, G_TYPE_INT, &x, G_TYPE_INT, &y,
                          G_TYPE_INT, &width, G_TYPE_INT, &height, G_TYPE_INVALID)) {
        valid = gvalidity == TRUE;
    } else {
        if (error) { // dbus_g_proxy_call may return FALSE and not set error despite what the doc says
            g_error_free(error);
        }
        valid = false;
        return QRect();
    }

    return QRect(x, y, width, height);
}

void MInputContextGlibDBusConnection::setRedirectKeys(bool enabled)
{
    if ((redirectKeysEnabled() != enabled) && activeContext()) {
        dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "setRedirectKeys",
                                   G_TYPE_BOOLEAN, enabled ? TRUE : FALSE,
                                   G_TYPE_INVALID);

        MInputContextConnection::setRedirectKeys(enabled);
    }
}

void MInputContextGlibDBusConnection::setDetectableAutoRepeat(bool enabled)
{
    if ((detectableAutoRepeat() != enabled) && activeContext()) {
        dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "setDetectableAutoRepeat",
                                   G_TYPE_BOOLEAN, enabled,
                                   G_TYPE_INVALID);

        MInputContextConnection::setDetectableAutoRepeat(enabled);
    }
}


void MInputContextGlibDBusConnection::copy()
{
    if (activeContext()) {
        dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "copy",
                                   G_TYPE_INVALID);
    }
}


void MInputContextGlibDBusConnection::paste()
{
    if (activeContext()) {
        dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "paste",
                                   G_TYPE_INVALID);
    }
}


void MInputContextGlibDBusConnection::setSelection(int start, int length)
{
    if (activeContext()) {
        dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "setSelection",
                                   G_TYPE_INT, start,
                                   G_TYPE_INT, length,
                                   G_TYPE_INVALID);
    }
}

void MInputContextGlibDBusConnection::updateInputMethodArea(const QRegion &region)
{
    if (activeContext()) {
        QRect rect = region.boundingRect();
        dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "updateInputMethodArea",
                                   G_TYPE_INT, rect.left(),
                                   G_TYPE_INT, rect.top(),
                                   G_TYPE_INT, rect.width(),
                                   G_TYPE_INT, rect.height(),
                                   G_TYPE_INVALID);
    }
}

void MInputContextGlibDBusConnection::notifyExtendedAttributeChanged(int id,
                                                                     const QString &target,
                                                                     const QString &targetItem,
                                                                     const QString &attribute,
                                                                     const QVariant &value)
{
    if (!activeContext()) {
        return;
    }
    GValue valueData = {0, {{0}, {0}}};
    if (!encodeVariant(&valueData, value)) {
        return;
    }

    dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "notifyExtendedAttributeChanged",
                               G_TYPE_INT, id,
                               G_TYPE_STRING, target.toUtf8().data(),
                               G_TYPE_STRING, targetItem.toUtf8().data(),
                               G_TYPE_STRING, attribute.toUtf8().data(),
                               G_TYPE_VALUE, &valueData,
                               G_TYPE_INVALID);
    g_value_unset(&valueData);
}

void MInputContextGlibDBusConnection::notifyExtendedAttributeChanged(const QList<int> &clientIds,
                                                                     int id,
                                                                     const QString &target,
                                                                     const QString &targetItem,
                                                                     const QString &attribute,
                                                                     const QVariant &value)
{
    GValue valueData = {0, {{0}, {0}}};
    if (!encodeVariant(&valueData, value)) {
        return;
    }
    Q_FOREACH (int clientId, clientIds) {
        dbus_g_proxy_call_no_reply(connectionObj(clientId)->inputContextProxy, "notifyExtendedAttributeChanged",
                                   G_TYPE_INT, id,
                                   G_TYPE_STRING, target.toUtf8().data(),
                                   G_TYPE_STRING, targetItem.toUtf8().data(),
                                   G_TYPE_STRING, attribute.toUtf8().data(),
                                   G_TYPE_VALUE, &valueData,
                                   G_TYPE_INVALID);
    }
    g_value_unset(&valueData);
}


void MInputContextGlibDBusConnection::pluginSettingsLoaded(int clientId, const QList<MImPluginSettingsInfo> &info)
{
    MDBusGlibICConnection *client = connectionObj(clientId);
    if (!client) {
        return;
    }

    GType settingsType;
    GPtrArray *settingsData;
    if (!encodeSettings(&settingsType, &settingsData, info)) {
        return;
    }

    dbus_g_proxy_call_no_reply(client->inputContextProxy, "pluginSettingsLoaded",
                               settingsType, settingsData,
                               G_TYPE_INVALID);
    dbus_g_type_collection_peek_vtable(settingsType)->base_vtable.free_func(settingsType, settingsData);
}


void MInputContextGlibDBusConnection::sendActivationLostEvent()
{
    if (activeContext()) {
        dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "activationLostEvent",
                                   G_TYPE_INVALID);
    }
}
