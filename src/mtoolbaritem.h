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


#ifndef MTOOLBARITEM_H
#define MTOOLBARITEM_H

#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>
#include <QSharedPointer>

#include <minputmethodnamespace.h>

class MToolbarItemPrivate;
class MToolbarItemActionPrivate;

/*! \ingroup maliitserver
 * \brief Defines action which could be performed by MToolbarItem.
 *
 * \sa MInputMethod::ActionType
 */
class MToolbarItemAction {
public:
    //! Construct empty object
    explicit MToolbarItemAction(MInputMethod::ActionType action = MInputMethod::ActionUndefined);

    virtual ~MToolbarItemAction();

    //! Action type, default value does nothing.
    MInputMethod::ActionType type() const;

    /*!
     * \brief Set action type.
     * \param type New action type.
     */
    void setType(MInputMethod::ActionType type);

    //! Keys which will be simulated by ActionSendKeySequence command
    QString keys() const;

    //! Set \a keys for ActionSendKeySequence command
    void setKeys(const QString &keys);

    //! Text which will be sent by ActionSendString command
    QString text() const;

    //! Set \a text for ActionSendString command
    void setText(const QString &text);

    /*!
     * \brief Command which will be sent by ActionSendCommand command.
     *
     * Warning: not implemented yet
     */
    QString command() const;

    //! Set \a command for ActionSendCommand command.
    void setCommand(const QString &command);

    //! Name of group wich will be affected by this action, if applicable.
    QString group() const;

    //! Set name of group wich will be affected by this action, if applicable.
    void setGroup(const QString &group);

private:
    Q_DECLARE_PRIVATE(MToolbarItemAction)

    MToolbarItemActionPrivate *const d_ptr;
};

/*!
 * \brief MToolbarItem represents a named item in a customized toolbar for virtual keyboard.
 */
class MToolbarItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString text   READ text    WRITE setText)
    Q_PROPERTY(QString textId READ textId  WRITE setTextId)
    Q_PROPERTY(bool visible   READ isVisible WRITE setVisible)
    Q_PROPERTY(bool toggle    READ toggle  WRITE setToggle)
    Q_PROPERTY(bool pressed   READ pressed WRITE setPressed)
    Q_PROPERTY(QString icon   READ icon    WRITE setIcon)
    Q_PROPERTY(int size       READ size    WRITE setSize)
    Q_PROPERTY(QString iconId READ iconId  WRITE setIconId)
    Q_PROPERTY(bool enabled   READ enabled WRITE setEnabled)
    Q_PROPERTY(Qt::Alignment contentAlignment READ contentAlignment WRITE setContentAlignment)
    Q_PROPERTY(bool highlighted READ highlighted WRITE setHighlighted)

public:
    /*!
     * \brief This enumeration defines which properties are maintained by MToolbarItem.
     * Names of items of this enumerations are self-descriptive and mostly matches
     * with names of qproperties defined for this object.
     */
    enum PropertyId {
        Nothing = 0, //!< Dummy value which does not have correponding property.
        Visible, // Item's visibility
        Text, // Item's text
        TextId, // Item's text id
        Toggle, // Item's ability to be togglable
        Pressed, // Item is pressed or released
        Highlighted, // Item is highlighted or not
        Enabled, // Item is enabled or disabled
        Icon, // Item's icon
        IconId, // Item's icon id
        ContentAlignment, // Alignment of item's content
        Size // Item's size
    };

    /*!
    * \brief Constructor
    */
    MToolbarItem(const QString &name, MInputMethod::ItemType type);

    /*!
     * \brief Copy constructor
     */
    MToolbarItem(const MToolbarItem&);

    /*!
    * \brief Destructor
    */
    virtual ~MToolbarItem();

    /*
     * \brief Assignment operator
     */
    const MToolbarItem &operator=(const MToolbarItem &other);

    /*!
     * \brief Returns the ItemType of the item.
     * \sa ItemType.
     */
    MInputMethod::ItemType type() const;

    /*!
     * \brief Returns the name of the item.
     */
    QString name() const;

    /*!
    * \brief Returns visibility of the item.
    */
    bool isVisible() const;

    /*!
    * \brief Sets the visibility of the item.
    */
    void setVisible(bool newVisible, bool explicitly);

    //! Returns text from item
    QString text() const;

    //! Returns text id
    QString textId() const;

    /*!
     * \brief Returns true if item could be toggled
     *
     * This attribute is applicable for buttons only.
     */
    bool toggle() const;

    /*!
     * \brief Defines whether item could be toggled or not
     *
     * This attribute is applicable for buttons only.
     */
    void setToggle(bool toggle);

    /*!
     * \brief Returns true if item is pressed
     *
     * This attribute is applicable for toggle buttons only.
     */
    bool pressed() const;

    //! Returns icon name
    QString icon() const;

    //! Sets icon name
    void setIcon(const QString &icon);

    //! Returns icon id
    QString iconId() const;

    //! Sets icon id
    void setIconId(const QString &iconId);

    //! Return alignment of content inside item
    Qt::Alignment contentAlignment() const;

    //! Set  alignment of content inside item
    void setContentAlignment(Qt::Alignment alignment);

    Qt::Alignment alignment() const;

    QList<QSharedPointer<const MToolbarItemAction> > actions() const;

    QList<QSharedPointer<MToolbarItemAction> > actions();

    MInputMethod::VisibleType showOn() const;

    void setShowOn(MInputMethod::VisibleType showOn);

    MInputMethod::VisibleType hideOn() const;

    void setHideOn(MInputMethod::VisibleType hideOn);

    QString group() const;

    void setGroup(const QString& group);

    int size() const;

    void setSize(int size);

    //! Return highlighted state for the item.
    bool highlighted() const;

    bool isCustom() const;

    void setCustom(bool custom);

    //! Return true if item is enabled; otherwise return false.
    bool enabled() const;

public Q_SLOTS:
    //! Sets new text for item
    void setText(const QString &text);

    //! Sets text id for item
    void setTextId(const QString &textId);

    /*!
     * \brief Sets presseed state for item
     *
     * This attribute is applicable for toggle buttons only.
     */
    void setPressed(bool pressed);

    /*!
     * \brief Set highlighted state for item
     *
     * This attribute is applicable for buttons only.
     */
    void setHighlighted(bool highlighted);

    /*!
     * \brief  If \a enabled is true, the item is enabled; otherwise, it is disabled.
     *
     * This attribute is applicable for buttons only.
     */
    void setEnabled(bool enabled);

    /*!
     * \brief Explictly set the visiblity for the item.
     */
    void setVisible(bool newVisible);

Q_SIGNALS:
    /*!
     * \brief Emitted when some property is changed
     * \param propertyName Specifies name of changed property
     *
     * WARNING: this signal is obsoleted and will be removed.
     * Please use
     */
    void propertyChanged(const QString &propertyName);

    /*!
     * \brief Emitted when some property is changed
     * \param propertyId Specifies identifier of changed property
     */
    void propertyChanged(PropertyId propertyId);

protected:
    //! Remove all actions from this item
    void clearActions();

    /*!
     * \brief Set alignment for item.
     *
     * Alignment could not be changed after toolbar loading.
     */
    void setAlignment(Qt::Alignment alignment);

    //! Add \a action to action list for this item.
    void append(const QSharedPointer<MToolbarItemAction> &action);

private:
    Q_DECLARE_PRIVATE(MToolbarItem)

    MToolbarItemPrivate *const d_ptr;

    friend class MToolbarData;
    friend class MToolbarDataPrivate;
    friend class ParseParameters;
    friend class Ut_MToolbarData;
};

Q_DECLARE_METATYPE(MToolbarItem::PropertyId);

#endif

