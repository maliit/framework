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


#ifndef MATTRIBUTEEXTENSIONMANAGER_H
#define MATTRIBUTEEXTENSIONMANAGER_H

#include <QObject>
#include <QHash>
#include <QSet>
#include <QPointer>
#include <QSharedPointer>

#include <maliit/namespace.h>

#include "mkeyoverridedata.h"
#include "mattributeextension.h"
#include "mattributeextensionid.h"
#include "mimsettings.h"

//! \internal
/*! \ingroup maliitserver
 \brief The MAttributeExtensionManager class manager the virtual keyboard attribute extensions.

  AttributeExtensionManager loads and manages not only the toolbars which are registered
  by applications, but also the copy/paste button and key overrides.
*/
class MAttributeExtensionManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MAttributeExtensionManager)

public:
    /*!
     * \brief Default constructor.
     */
    MAttributeExtensionManager();

    /*!
     *\brief Destructor.
     */
    virtual ~MAttributeExtensionManager();

    /*!
     * \brief Register an input method attribute extension which is defined in \a fileName with the unique identifier \a id.
     * AttributeExtensionManager can load a attribute extension's content according \a id and \a fileName, and cache it for the
     * future use. The \a id should be unique, and the \a fileName is the absolute file name of the attribute extension.
     */
    void registerAttributeExtension(const MAttributeExtensionId &id, const QString &fileName);

    /*!
     * \brief Unregister an input method attribute extension which unique identifier is \a id.
     * AttributeExtensionManager will remove the cached widget according \a id.
     */
    void unregisterAttributeExtension(const MAttributeExtensionId &id);

    /*!
     * \brief Sets the \a attribute for the \a item in the custom toolbar which has the unique \a id to \a value.
     */
    void setToolbarItemAttribute(const MAttributeExtensionId &id, const QString &item,
                                 const QString &attribute, const QVariant &value);

    /*!
     *\brief Returns widget data definition for gived \a id.
     */
    QSharedPointer<MAttributeExtension> attributeExtension(const MAttributeExtensionId &id) const;

    /*!
     *\brief Returns key overrides definition for gived \a id.
     */
    QMap<QString, QSharedPointer<MKeyOverride> > keyOverrides(const MAttributeExtensionId &id) const;

    /*!
     *\brief Returns whether registered attribute extensions contain \a id.
     */
    bool contains(const MAttributeExtensionId &id) const;

    /*!
     *\brief Sets the \a attribute of the \a targetItem in the attribute extension \a target which has unique\a id to \a value.
     */
    void setExtendedAttribute(const MAttributeExtensionId &id,
                              const QString &target,
                              const QString &targetItem,
                              const QString &attribute,
                              const QVariant &value);
public Q_SLOTS:
    /*!
     * \brief Set copy/paste button state: hide it, show copy or show paste
     *  \param copyAvailable True if text is selected
     *  \param pasteAvailable True if clipboard content is not empty
     */
    void setCopyPasteState(bool copyAvailable, bool pasteAvailable);

    void handleClientDisconnect(unsigned int clientId);
    void handleAttributeExtensionRegistered(unsigned int clientId, int id, const QString &attributeExtension);
    void handleAttributeExtensionUnregistered(unsigned int clientId, int id);
    void handleExtendedAttributeUpdate(unsigned int clientId, int id,
                                       const QString &target, const QString &targetName,
                                       const QString &attribute, const QVariant &value);
    void handleWidgetStateChanged(unsigned int clientId, const QMap<QString, QVariant> &newState, const QMap<QString, QVariant> &oldState, bool focusChanged);

Q_SIGNALS:
    //! This signal is emited when a new key override is created.
    void keyOverrideCreated();

    //! Emitted when attribute extension has changed
    void attributeExtensionIdChanged(const MAttributeExtensionId &id);

    /*!
     * \brief This signal is emitted when application wants to change global attribute
     * that affects the whole input methods framework.
     * \param id the attribute extension id
     * \param targetItem Item name
     * \param attribute Attribute name
     * \param value New attribute value
     */
    void globalAttributeChanged(const MAttributeExtensionId &id,
                                const QString &targetItem,
                                const QString &attribute,
                                const QVariant &value);

    /*
     * \brief Emitted when attribute is changed by framework or plugin.
     * \param id the unique identifier of a registered extended attribute.
     * \param target a string specifying the target for the attribute.
     * \param targetItem the item name.
     * \param attribute attribute to be changed.
     * \param value new value.
     * \sa handleExtendedAttributeUpdate()
     */
    void notifyExtensionAttributeChanged(int id,
                                         const QString &target,
                                         const QString &targetItem,
                                         const QString &attribute,
                                         const QVariant &value);

private:
    /*!
     * \brief Returns a list of the id for all attribute extensions' ids.
     */
    QList<MAttributeExtensionId> attributeExtensionIdList() const;

    typedef QHash<MAttributeExtensionId, QSharedPointer<MAttributeExtension> > AttributeExtensionContainer;
    //! all registered attribute extensions
    AttributeExtensionContainer attributeExtensions;

    MAttributeExtensionId attributeExtensionId; //current attribute extension id
    QSet<MAttributeExtensionId> attributeExtensionIds; //all attribute extension ids

    //! Copy/paste button status
    Maliit::CopyPasteState copyPasteStatus;

    friend class Ut_MAttributeExtensionManager;
};

//! \internal_end

#endif

