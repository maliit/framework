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

#include <MNamespace>
#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>
#include <QSharedPointer>

#include <minputmethodnamespace.h>

class MToolbarItemPrivate;
class MToolbarItemActionPrivate;

/*!
 * \brief MToolbarItemAction defines action which could be performed by MToolbarItem.
 *
 * See also MInputMethod::ActionType
 */
struct MToolbarItemAction {
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

public:
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
    void setVisible(bool);

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

    Qt::Alignment alignment() const;

    int priority() const;


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

public slots:
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

signals:
    /*!
     * \brief Emitted when some property is changed
     * \param propertyName Specifies name of changed property
     */
    void propertyChanged(const QString &propertyName);

protected:
    //! Remove all actions from this item
    void clearActions();

    /*!
     * \brief Set priority for item. Most prioritized items are shown at
     * borders of the screen.
     *
     * Priority could not be changed after toolbar loading.
     */
    void setPriority(int priority);

    /*!
     * \brief Set alignment for item.
     *
     * Alignment could not be changed after toolbar loading.
     */
    void setAlignment(Qt::Alignment alignment);

    //! Add \a action to action list for this item.
    void append(QSharedPointer<MToolbarItemAction> action);

private:
    Q_DECLARE_PRIVATE(MToolbarItem)

    MToolbarItemPrivate *const d_ptr;

    friend class MToolbarData;
    friend class ParseParameters;
    friend class Ut_MToolbarData;
};

#endif

