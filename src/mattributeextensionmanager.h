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
#include <QPointer>

#include "mkeyoverridedata.h"
#include "mtoolbardata.h"
#include "mattributeextension.h"
#include "minputmethodnamespace.h"
#include "mimsettings.h"

class MAttributeExtensionId;

//! \internal
/*!
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
     *\brief Destructor.
     */
    virtual ~MAttributeExtensionManager();

    //! \brief Get singleton instance
    //! \return singleton instance
    static MAttributeExtensionManager &instance();

    //! \brief Destroy singleton
    static void destroyInstance();

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
     *\brief Returns toolbar definition for gived \a id.
     */
    QSharedPointer<MToolbarData> toolbarData(const MAttributeExtensionId &id) const;


    /*!
     *\brief Returns key overrides definition for gived \a id.
     */
    QMap<QString, QSharedPointer<MKeyOverride> > keyOverrides(const MAttributeExtensionId &id) const;

    /*!
     *\brief Returns whether registered attribute extensions contain \a id.
     */
    bool contains(const MAttributeExtensionId &id) const;

    /*!
     * \brief Set copy/paste button state: hide it, show copy or show paste
     *  \param copyAvailable True if text is selected
     *  \param pasteAvailable True if clipboard content is not empty
     */
    void setCopyPasteState(bool copyAvailable, bool pasteAvailable);

    /*!
     *\brief Sets the \a attribute of the \a targetItem in the attribute extension \a target which has unique\a id to \a value.
     */
    void setExtendedAttribute(const MAttributeExtensionId &id,
                              const QString &target,
                              const QString &targetItem,
                              const QString &attribute,
                              const QVariant &value);
private slots:
    //! \brief Handle preferred_domain GConf setting updates.
    void handlePreferredDomainUpdate();

signals:
    //! This signal is emited when a new key override is created.
    void keyOverrideCreated();

private:
    /*!
     * \brief Default constructor.
     */
    MAttributeExtensionManager();

    /*!
     * \brief Returns a list of the id for all attribute extensions' ids.
     */
    QList<MAttributeExtensionId> attributeExtensionIdList() const;


    //! Create standard widget data which contains standard attribute extension
    void createStandardObjects();

    /*!
     * \brief Add standard toolbar items to the custom toolbar.
     * \param toolbarData Custom toolbar data
     */
    void addStandardButtons(const QSharedPointer<MToolbarData> &toolbarData);

    //! This overloaded function provided for convinience
    void addStandardButtons(const QSharedPointer<MToolbarLayout> &layout,
                            const QSharedPointer<MToolbarData> &toolbarData);

    //! \brief Update the text of a button named _domain in \a toolbar to match the configuration.
    void updateDomain(QSharedPointer<MToolbarData> &toolbar);

    typedef QHash<MAttributeExtensionId, QSharedPointer<MAttributeExtension> > AttributeExtensionContainer;
    //! all registered attribute extensions
    AttributeExtensionContainer attributeExtensions;

    //! Standard close button
    QSharedPointer<MToolbarItem> close;

    //! Standard copy/paste button
    QSharedPointer<MToolbarItem> copyPaste;

    //! Standard AttributeExtension contaning standard toolbar buttons only
    QSharedPointer<MAttributeExtension> standardAttributeExtension;

    //! Copy/paste button status
    MInputMethod::CopyPasteState copyPasteStatus;

    //! Preferred domain for URL and Email toolbar domain buttons.
    MImSettings preferredDomainSetting;

    //! Singleton instance
    static MAttributeExtensionManager *attributeExtensionMgrInstance;

    friend class Ut_MAttributeExtensionManager;
};

inline MAttributeExtensionManager &MAttributeExtensionManager::instance()
{
    if (!attributeExtensionMgrInstance) {
        attributeExtensionMgrInstance = new MAttributeExtensionManager;
    }
    return *attributeExtensionMgrInstance;
}
//! \internal_end

#endif

