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


#ifndef MTOOLBARMANAGER_H
#define MTOOLBARMANAGER_H

#include <QObject>
#include <QHash>
#include <QPointer>

#include <MGConfItem>

#include "mtoolbardata.h"
#include "minputmethodnamespace.h"

class MToolbarId;

//! \internal
/*!
 \brief The MToolbarManager class manager the virtual keyboard toolbar.

  ToolbarManager loads and manages not only the toolbars which are registered
  by applications, but also the copy/paste button.
*/
class MToolbarManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MToolbarManager)

public:
    /*!
     *\brief Destructor.
     */
    virtual ~MToolbarManager();

    //! \brief Get singleton instance
    //! \return singleton instance
    static MToolbarManager &instance();

    //! \brief Create singleton
    static void createInstance();

    //! \brief Destroy singleton
    static void destroyInstance();

    /*!
     * \brief Register an input method toolbar which is defined in \a fileName with the unique identifier \a id.
     * ToolbarManager can load a custom toolbar's content according \a id and \a fileName, and cache it for the
     * future use. The \a id should be unique, and the \a fileName is the absolute file name of the custom toolbar.
     */
    void registerToolbar(const MToolbarId &id, const QString &fileName);

    /*!
     * \brief Unregister an input method \a toolbar which unique identifier is \a id.
     * ToolbarManager will remove the cached toolbar according \a id.
     */
    void unregisterToolbar(const MToolbarId &id);

    /*!
     * \brief Sets the \a attribute for the \a item in the custom toolbar which has the unique \a id to \a value.
     */
    void setToolbarItemAttribute(const MToolbarId &id, const QString &item,
                                 const QString &attribute, const QVariant &value);

    /*!
     *\brief Returns toolbar definition for gived \a id.
     */
    QSharedPointer<MToolbarData> toolbarData(const MToolbarId &id) const;

    /*!
     *\brief Returns whether registered toolbars contain toolbar \a id.
     */
    bool contains(const MToolbarId &id) const;

    /*!
     * \brief Set copy/paste button state: hide it, show copy or show paste
     *  \param copyAvailable True if text is selected
     *  \param pasteAvailable True if clipboard content is not empty
     */
    void setCopyPasteState(bool copyAvailable, bool pasteAvailable);

    /*!
     *\brief Returns key overrides definition for gived \a id.
     */
    QList<QSharedPointer<MKeyOverride> > keyOverrides(const MToolbarId &id) const;

    /*!
     * \brief Sets the \a attribute for the \a keyId in the custom widget which has the unique \a id to \a value.
     */
    void setKeyAttribute(const MToolbarId &id, const QString &keyId,
                         const QString &attribute, const QVariant &value);

private slots:
    //! \brief Handle preferred_domain GConf setting updates.
    void handlePreferredDomainUpdate();

private:
    /*!
     * \brief Default constructor.
     */
    MToolbarManager();

    /*!
     * \brief Returns a list of the id for all toolbars.
     */
    QList<MToolbarId> toolbarList() const;

    /*!
     * \brief Returns new toolbar which is loaded from specified file \a name.
     */
    QSharedPointer<MToolbarData> createToolbar(const QString &name);

    //! Create standard toolbar items and standard toolbar
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

    typedef QHash<MToolbarId, QSharedPointer<MToolbarData> > ToolbarContainer;
    //! all registered toolbars
    ToolbarContainer toolbars;

    //! Standard close button
    QSharedPointer<MToolbarItem> close;

    //! Standard copy/paste button
    QSharedPointer<MToolbarItem> copyPaste;

    //! Toolbar contaning standard buttons only
    QSharedPointer<MToolbarData> standardToolbar;

    //! Copy/paste button status
    MInputMethod::CopyPasteState copyPasteStatus;

    //! Preferred domain for URL and Email toolbar domain buttons.
    MGConfItem preferredDomainSetting;

    //! Singleton instance
    static MToolbarManager *toolbarMgrInstance;

    friend class Ut_MToolbarManager;
};

inline MToolbarManager &MToolbarManager::instance()
{
    Q_ASSERT(toolbarMgrInstance);
    return *toolbarMgrInstance;
}
//! \internal_end

#endif

