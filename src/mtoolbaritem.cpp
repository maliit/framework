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



#include <QtAlgorithms>
#include "mtoolbaritem.h"
#include "mtoolbaritem_p.h"

MToolbarItemActionPrivate::MToolbarItemActionPrivate()
    : type(MInputMethod::ActionUndefined)
{
}

MToolbarItemPrivate::MToolbarItemPrivate()
    : type(MInputMethod::ItemUndefined),
      priority(0),
      showOn(MInputMethod::VisibleUndefined),
      hideOn(MInputMethod::VisibleUndefined),
      alignment(Qt::AlignRight),
      visible(true),
      toggle(false),
      pressed(false),
      size(100)
{
}

void MToolbarItemPrivate::assign(const MToolbarItemPrivate &other)
{
    name      = other.name;
    group     = other.group;
    text      = other.text;
    textId    = other.textId;
    icon      = other.icon;
    iconId    = other.iconId;
    type      = other.type;
    priority  = other.priority;
    showOn    = other.showOn;
    hideOn    = other.hideOn;
    alignment = other.alignment;
    visible   = other.visible;
    toggle    = other.toggle;
    pressed   = other.pressed;
    size      = other.size;
    actions   = other.actions;
}

MToolbarItemAction::MToolbarItemAction(MInputMethod::ActionType t)
    : d_ptr(new MToolbarItemActionPrivate)
{
    d_ptr->type = t;
}

MToolbarItemAction::~MToolbarItemAction()
{
    delete d_ptr;
}

MInputMethod::ActionType MToolbarItemAction::type() const
{
    Q_D(const MToolbarItemAction);

    return d->type;
}

void MToolbarItemAction::setType(MInputMethod::ActionType type)
{
    Q_D(MToolbarItemAction);

    d->type = type;
}

QString MToolbarItemAction::keys() const
{
    Q_D(const MToolbarItemAction);

    return d->keys;
}

void MToolbarItemAction::setKeys(const QString &keys)
{
    Q_D(MToolbarItemAction);

    d->keys = keys;
}

QString MToolbarItemAction::text() const
{
    Q_D(const MToolbarItemAction);

    return d->text;
}

void MToolbarItemAction::setText(const QString &text)
{
    Q_D(MToolbarItemAction);

    d->text = text;
}

QString MToolbarItemAction::command() const
{
    Q_D(const MToolbarItemAction);

    return d->command;
}

void MToolbarItemAction::setCommand(const QString &command)
{
    Q_D(MToolbarItemAction);

    d->command = command;
}

QString MToolbarItemAction::group() const
{
    Q_D(const MToolbarItemAction);

    return d->group;
}

void MToolbarItemAction::setGroup(const QString &group)
{
    Q_D(MToolbarItemAction);

    d->group = group;
}

MToolbarItem::MToolbarItem(const QString &name, MInputMethod::ItemType type)
    : QObject(),
    d_ptr(new MToolbarItemPrivate)
{
    d_ptr->type = type;
    d_ptr->name = name;
}

MToolbarItem::MToolbarItem(const MToolbarItem &other)
    : QObject(),
    d_ptr(new MToolbarItemPrivate)
{
    *this = other;
}

MToolbarItem::~MToolbarItem()
{
    delete d_ptr;
}

const MToolbarItem & MToolbarItem::operator=(const MToolbarItem &other)
{
    Q_D(MToolbarItem);

    d->assign(*other.d_ptr);

    return *this;
}

MInputMethod::ItemType MToolbarItem::type() const
{
    Q_D(const MToolbarItem);

    return d->type;
}

QString MToolbarItem::name() const
{
    Q_D(const MToolbarItem);

    return d->name;
}

bool MToolbarItem::isVisible() const
{
    Q_D(const MToolbarItem);

    return d->visible;
}

void MToolbarItem::setVisible(bool v)
{
    Q_D(MToolbarItem);

    if (d->visible != v) {
        d->visible = v;
        emit propertyChanged("visible");
    }
}

QString MToolbarItem::text() const
{
    Q_D(const MToolbarItem);

    return d->text;
}

void MToolbarItem::setText(const QString &text)
{
    Q_D(MToolbarItem);

    if (d->text != text) {
        d->text = text;
        emit propertyChanged("text");
    }
}

QString MToolbarItem::textId() const
{
    Q_D(const MToolbarItem);

    return d->textId;
}

void MToolbarItem::setTextId(const QString &textId)
{
    Q_D(MToolbarItem);
    if (d->textId != textId) {
        d->textId = textId;
        emit propertyChanged("textId");
    }
}

bool MToolbarItem::toggle() const
{
    Q_D(const MToolbarItem);

    return d->toggle;
}

void MToolbarItem::setToggle(bool toggle)
{
    Q_D(MToolbarItem);

    if (d->toggle != toggle) {
        d->toggle = toggle;
        emit propertyChanged("toggle");
    }
}

bool MToolbarItem::pressed() const
{
    Q_D(const MToolbarItem);

    return d->pressed;
}

void MToolbarItem::setPressed(bool pressed)
{
    Q_D(MToolbarItem);

    if (d->pressed != pressed) {
        d->pressed = pressed;
        emit propertyChanged("pressed");
    }
}

QString MToolbarItem::icon() const
{
    Q_D(const MToolbarItem);

    return d->icon;
}

void MToolbarItem::setIcon(const QString &icon)
{
    Q_D(MToolbarItem);

    if (d->icon != icon) {
        d->icon = icon;
        emit propertyChanged("icon");
    }
}

QString MToolbarItem::iconId() const
{
    Q_D(const MToolbarItem);

    return d->iconId;
}

void MToolbarItem::setIconId(const QString &iconId)
{
    Q_D(MToolbarItem);

    if (d->iconId != iconId) {
        d->iconId = iconId;
        emit propertyChanged("iconId");
    }
}

Qt::Alignment MToolbarItem::alignment() const
{
    Q_D(const MToolbarItem);

    return d->alignment;
}

QList<QSharedPointer<const MToolbarItemAction> >
MToolbarItem::actions() const
{
    Q_D(const MToolbarItem);
    QList<QSharedPointer<const MToolbarItemAction> > result;

    foreach (const QSharedPointer<const MToolbarItemAction> action, d->actions) {
        result.append(action);
    }
    return result;
}

QList<QSharedPointer<MToolbarItemAction> >
MToolbarItem::actions()
{
    Q_D(const MToolbarItem);

    return d->actions;
}

MInputMethod::VisibleType MToolbarItem::showOn() const
{
    Q_D(const MToolbarItem);

    return d->showOn;
}

void MToolbarItem::setShowOn(MInputMethod::VisibleType showOn)
{
    Q_D(MToolbarItem);

    d->showOn = showOn;
}

MInputMethod::VisibleType MToolbarItem::hideOn() const
{
    Q_D(const MToolbarItem);

    return d->hideOn;
}

void MToolbarItem::setHideOn(MInputMethod::VisibleType hideOn)
{
    Q_D(MToolbarItem);

    d->hideOn = hideOn;
}

QString MToolbarItem::group() const
{
    Q_D(const MToolbarItem);

    return d->group;
}

void MToolbarItem::setGroup(const QString &group)
{
    Q_D(MToolbarItem);

    d->group = group;
}

int MToolbarItem::size() const
{
    Q_D(const MToolbarItem);

    return d->size;
}

void MToolbarItem::setSize(int size)
{
    Q_D(MToolbarItem);

    if (d->size != size) {
        d->size = size;
        emit propertyChanged("size");
    }
}

void MToolbarItem::clearActions()
{
    Q_D(MToolbarItem);

    d->actions.clear();
}

void MToolbarItem::setAlignment(Qt::Alignment alignment)
{
    Q_D(MToolbarItem);

    d->alignment = alignment;
}

void MToolbarItem::append(const QSharedPointer<MToolbarItemAction> &action)
{
    Q_D(MToolbarItem);

    d->actions.append(action);
}

