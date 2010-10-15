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


#include "mtoolbardata.h"
#include "mtoolbardata_p.h"
#include "mtoolbarrow.h"
#include "mtoolbarlayout.h"

#include <QFile>
#include <QFileInfo>
#include <QDomDocument>
#include <QDebug>

namespace
{
    const QString ToolbarConfigurationPath   = QString::fromLatin1("/usr/share/meegoimframework/imtoolbars/");

    const QString ImTagToolbar               = QString::fromLatin1("toolbar");
    const QString ImTagLocked                = QString::fromLatin1("locked");
    const QString ImTagLockedDefValue        = QString::fromLatin1("false");
    const QString ImTagLayout                = QString::fromLatin1("layout");
    const QString ImTagRow                   = QString::fromLatin1("row");
    const QString ImTagButton                = QString::fromLatin1("button");
    const QString ImTagLabel                 = QString::fromLatin1("label");
    const QString ImTagActions               = QString::fromLatin1("actions");
    const QString ImTagName                  = QString::fromLatin1("name");
    const QString ImTagGroup                 = QString::fromLatin1("group");
    const QString ImTagPriority              = QString::fromLatin1("priority");
    const QString ImTagOrientation           = QString::fromLatin1("orientation");
    const QString ImTagShowOn                = QString::fromLatin1("showon");
    const QString ImTagHideOn                = QString::fromLatin1("hideon");
    const QString ImTagAlignment             = QString::fromLatin1("alignment");
    const QString ImTagIcon                  = QString::fromLatin1("icon");
    const QString ImTagSize                  = QString::fromLatin1("size");
    const QString ImTagIconId                = QString::fromLatin1("icon_id");
    const QString ImTagText                  = QString::fromLatin1("text");
    const QString ImTagTextId                = QString::fromLatin1("text_id");
    const QString ImTagToggle                = QString::fromLatin1("toggle");
    const QString ImTagPressed               = QString::fromLatin1("pressed");
    const QString ImTagSelectedText          = QString::fromLatin1("selectedtext");
    const QString ImTagAlways                = QString::fromLatin1("always");
    const QString ImTagLeft                  = QString::fromLatin1("left");
    const QString ImTagRight                 = QString::fromLatin1("right");
    const QString ImTagSendKeySequence       = QString::fromLatin1("sendkeysequence");
    const QString ImTagSendString            = QString::fromLatin1("sendstring");
    const QString ImTagSendCommand           = QString::fromLatin1("sendcommand");
    const QString ImTagCopy                  = QString::fromLatin1("copy");
    const QString ImTagPaste                 = QString::fromLatin1("paste");
    const QString ImTagShowGroup             = QString::fromLatin1("showgroup");
    const QString ImTagHideGroup             = QString::fromLatin1("hidegroup");
    const QString ImTagKeySequence           = QString::fromLatin1("keysequence");
    const QString ImTagString                = QString::fromLatin1("string");
    const QString ImTagCommand               = QString::fromLatin1("command");
    const QString ImTagOrientationLandscape  = QString::fromLatin1("landscape");
    const QString ImTagOrientationDefValue   = ImTagOrientationLandscape;
    const QString ImTagOrientationPortrait   = QString::fromLatin1("portrait");
    const QString ImTagVersion               = QString::fromLatin1("version");
    const QString ImTagVersionDefValue       = QString::fromLatin1("0");
    const QString ImTagCopyPaste             = QString::fromLatin1("copypaste");
    const QString ImTagClose                 = QString::fromLatin1("close");
    const QString ImTagVisible               = QString::fromLatin1("visible");
    const QString ImTagVisibleDefValue       = QString::fromLatin1("true");
    const QString ImTagRefuse                = QString::fromLatin1("refuse");

    const QChar NameSeparator(',');
}

struct MTBParseParameters {
    MTBParseParameters();

    //! Contains true if current XML tag was successfully parsed
    bool validTag;

    QString fileName;

    //! Contains version number of XML structure specification.
    int version;

    QSharedPointer<MToolbarLayout> currentLayout;
    QSharedPointer<MToolbarRow>    currentRow;
    QSharedPointer<MToolbarItem>   currentItem;
};

struct MTBParseStructure {
    MTBParseStructure(const QString &name, MToolbarData::TagParser p);

    QString tagName;
    MToolbarData::TagParser parser;
};

MTBParseParameters::MTBParseParameters()
    : validTag(true),
      version(0)
{
}

MTBParseStructure::MTBParseStructure(const QString &name, MToolbarData::TagParser p)
    : tagName(name),
      parser(p)
{
}

MToolbarDataPrivate::MToolbarDataPrivate()
    : locked(false),
      custom(false),
      visible(true)
{
}

// Actual class implementation

MToolbarData::MToolbarData()
    : d_ptr(new MToolbarDataPrivate)
{
}

MToolbarData::~MToolbarData()
{
    delete d_ptr;
}

bool MToolbarData::loadToolbarXml(const QString &fileName)
{
    Q_D(MToolbarData);
    QString absoluteFileName = fileName;
    QFileInfo info(absoluteFileName);
    if (info.isRelative())
        absoluteFileName = ToolbarConfigurationPath + info.fileName();
    if (!QFile::exists(absoluteFileName)) {
        qDebug() << __PRETTY_FUNCTION__ << "can not find file:" << absoluteFileName;
        return false;
    }

    MTBParseParameters params;
    params.fileName = absoluteFileName;
    d->toolbarFileName = absoluteFileName;
    bool valid = true;
    QFile infile(absoluteFileName);
    QString errorStr;
    int errorLine;
    int errorColumn;
    QDomDocument doc;
    if (!infile.open(QIODevice::ReadOnly)) {
        qWarning() << __PRETTY_FUNCTION__ << "Unable to open toolbar xml file" << absoluteFileName;
        return false;
    }
    if (!doc.setContent((QIODevice *)(&infile), true, &errorStr, &errorLine,
                        &errorColumn)) {
        qWarning() << __PRETTY_FUNCTION__ << "can not parse xml" << absoluteFileName
                   << "error line:" << errorLine << ", column:" << errorColumn;
        infile.close();
        return false;
    }

    const QDomElement root = doc.documentElement();
    //check the root tag
    if (!root.isNull() && root.tagName() != ImTagToolbar) {
        qWarning() << __PRETTY_FUNCTION__
                   << "wrong format xml" << absoluteFileName << "for virtual keyboard tool bar";
        valid = false;
    } else {
        parseTagToolbar(root, params);
        valid = params.validTag;
    }

    infile.close();

    return valid;
}

QList<QSharedPointer<MToolbarItem> > MToolbarData::items() const
{
    Q_D(const MToolbarData);

    return d->items.values();
}

QSharedPointer<const MToolbarLayout> MToolbarData::layout(M::Orientation orientation) const
{
    Q_D(const MToolbarData);

    switch(orientation) {
    case M::Landscape:
        return d->layoutLandscape;
    case M::Portrait:
        if (d->layoutPortrait) {
            return d->layoutPortrait;
        }
        return d->layoutLandscape;
    default:
        return QSharedPointer<const MToolbarLayout>();
    }
}

bool MToolbarData::locked() const
{
    Q_D(const MToolbarData);

    return d->locked;
}

bool MToolbarData::isCustom() const
{
    Q_D(const MToolbarData);

    return d->custom;
}

bool MToolbarData::isVisible() const
{
    Q_D(const MToolbarData);

    return d->visible;
}

bool MToolbarData::append(const QSharedPointer<MToolbarRow> &row,
                          const QSharedPointer<MToolbarItem> &item)
{
    Q_D(MToolbarData);

    if ((!layout(M::Landscape) || !layout(M::Landscape)->rows().contains(row))
       && (!layout(M::Portrait) || !layout(M::Portrait)->rows().contains(row))) {
        return false;
    }

    if (row->items().contains(item)) {
        return false;
    }

    row->append(item);
    d->items.insert(item->name(), item);

    return true;
}

QSharedPointer<MToolbarItem> MToolbarData::item(const QString &name) const
{
    Q_D(const MToolbarData);

    return d->items.value(name);
}

void MToolbarData::sort()
{
    Q_D(MToolbarData);

    sort(d->layoutLandscape);
    sort(d->layoutPortrait);
}

void MToolbarData::sort(const QSharedPointer<MToolbarLayout> &layout)
{
    if (!layout) {
        return;
    }

    foreach (QSharedPointer<MToolbarRow> row, layout->rows())
    {
        row->sort();
    }
}

QStringList MToolbarData::refusedNames() const
{
    Q_D(const MToolbarData);

    return d->refusedNames;
}

void MToolbarData::setCustom(bool custom)
{
    Q_D(MToolbarData);

    d->custom = custom;
}

QSharedPointer<MToolbarItem> MToolbarData::getOrCreateItemByName(const QString &name,
                                                                 MInputMethod::ItemType type)
{
    Q_D(MToolbarData);
    MToolbarDataPrivate::Items::iterator iterator(d->items.find(name));
    QSharedPointer<MToolbarItem> result;

    if (iterator != d->items.end()) {
        result = *iterator;
    } else {
        result = QSharedPointer<MToolbarItem>(new MToolbarItem(name, type));
        d->items.insert(name, result);
    }

    return result;
}

Qt::Alignment MToolbarData::alignment(const QString &alignmentString)
{
    Qt::Alignment align = Qt::AlignRight;
    if (alignmentString == ImTagLeft)
        align = Qt::AlignLeft;
    else if (alignmentString == ImTagRight)
        align = Qt::AlignRight;
    return align;
}

M::Orientation MToolbarData::orientation(const QString &orientationString)
{
    M::Orientation orient = M::Portrait;
    if (orientationString == ImTagOrientationLandscape)
        orient = M::Landscape;
    return orient;
}

MInputMethod::VisibleType MToolbarData::visibleType(const QString &visibleTypeString)
{
    MInputMethod::VisibleType type = MInputMethod::VisibleUndefined;
    if (visibleTypeString == ImTagSelectedText)
        type = MInputMethod::VisibleWhenSelectingText;
    else if (visibleTypeString == ImTagAlways)
        type = MInputMethod::VisibleAlways;
    return type;
}

void MToolbarData::parseAttribute(SetString setter, const QDomElement &element,
                                  const QString &attributeName, const MTBParseParameters &params)
{
    if (element.hasAttribute(attributeName)) {
        MToolbarItem &item = *params.currentItem;
        (item.*setter)(element.attribute(attributeName));
    }
}

void MToolbarData::parseAttribute(SetVisibleType setter, const QDomElement &element,
                                  const QString &attributeName, const MTBParseParameters &params)
{
    if (element.hasAttribute(attributeName)) {
        MToolbarItem &item = *params.currentItem;
        (item.*setter)(visibleType(element.attribute(attributeName)));
    }
}

void MToolbarData::parseAttribute(SetBool setter, const QDomElement &element,
                                  const QString &attributeName, const MTBParseParameters &params)
{
    if (element.hasAttribute(attributeName)) {
        bool value = (element.attribute(attributeName) == "true") ? true : false;
        MToolbarItem &item = *params.currentItem;
        (item.*setter)(value);
    }
}

void MToolbarData::parseAttribute(SetAlignment setter, const QDomElement &element,
                                  const QString &attributeName, const MTBParseParameters &params)
{
    if (element.hasAttribute(attributeName)) {
        MToolbarItem &item = *params.currentItem;
        (item.*setter)(alignment(element.attribute(attributeName)));
    }
}

void MToolbarData::parseAttribute(SetInt setter, const QDomElement &element,
                                  const QString &attributeName, const MTBParseParameters &params)
{
    if (element.hasAttribute(attributeName)) {
        MToolbarItem &item = *params.currentItem;
        (item.*setter)(element.attribute(attributeName).toInt());
    }
}

void MToolbarData::parseTagToolbar(const QDomElement &element, MTBParseParameters &params)
{
    Q_D(MToolbarData);

    d->locked = (element.attribute(ImTagLocked, ImTagLockedDefValue) == "true") ? true : false;
    d->visible = (element.attribute(ImTagVisible, ImTagVisibleDefValue) == "true") ? true : false;
    d->refusedNames = element.attribute(ImTagRefuse).split(NameSeparator);
    params.version = element.attribute(ImTagVersion, ImTagVersionDefValue).toInt();

    if (params.version == 1) {
        const MTBParseStructure parser(ImTagLayout, &MToolbarData::parseTagLayout);
        parseChildren(element, params, &parser);
    } else if (params.version == 0) {
        QSharedPointer<MToolbarLayout> layout(new MToolbarLayout());
        QSharedPointer<MToolbarRow> row(new MToolbarRow);

        d->layoutLandscape = layout;
        params.currentLayout = layout;

        params.currentLayout->append(row);
        params.currentRow = row;

        const MTBParseStructure parsers[2] = {
            MTBParseStructure(ImTagButton, &MToolbarData::parseTagButton),
            MTBParseStructure(ImTagLabel,  &MToolbarData::parseTagLabel),
        };

        parseChildren(element, params, parsers, 2);
    } else {
        qWarning() << __PRETTY_FUNCTION__ << "Invalid version number" << params.version;
        params.validTag = false;
    }
}

void MToolbarData::parseTagLayout(const QDomElement &element, MTBParseParameters &params)
{
    Q_D(MToolbarData);
    const QString attribute = element.attribute(ImTagOrientation, ImTagOrientationDefValue);
    const M::Orientation orientationData = orientation(attribute);
    QSharedPointer<MToolbarLayout> layout = this->layout(orientationData).constCast<MToolbarLayout>();

    if (layout && layout->orientation() == orientationData) {
        qWarning() << __PRETTY_FUNCTION__ << "Duplicated layout for" << attribute;
        params.validTag = false;
        return;
    }

    layout = QSharedPointer<MToolbarLayout>(new MToolbarLayout(orientationData));
    switch (orientationData) {
    case M::Landscape:
        d->layoutLandscape = layout;
        break;
    case M::Portrait:
        d->layoutPortrait = layout;
        break;
    default:
        Q_ASSERT(0); // should never happen
    }
    params.currentLayout = layout;

    const MTBParseStructure parser(ImTagRow, &MToolbarData::parseTagRow);

    parseChildren(element, params, &parser);
}

void MToolbarData::parseTagRow(const QDomElement &element, MTBParseParameters &params)
{
    QSharedPointer<MToolbarRow> row(new MToolbarRow);

    params.currentLayout->append(row);
    params.currentRow = row;

    const MTBParseStructure parsers[2] = {
        MTBParseStructure(ImTagButton, &MToolbarData::parseTagButton),
        MTBParseStructure(ImTagLabel,  &MToolbarData::parseTagLabel),
    };

    parseChildren(element, params, parsers, 2);
}

void MToolbarData::parseTagButton(const QDomElement &element, MTBParseParameters &params)
{
    const QString name = element.attribute(ImTagName);

    if (name.isEmpty()) {
        return;
    }

    QSharedPointer<MToolbarItem> item = getOrCreateItemByName(name, MInputMethod::ItemButton);

    if(item->type() != MInputMethod::ItemButton) {
        return;
    }

    setCustom(true);
    params.currentRow->append(item);
    params.currentItem = item;
    parseAttribute(&MToolbarItem::setText,      element, ImTagText,      params);
    parseAttribute(&MToolbarItem::setGroup,     element, ImTagGroup,     params);
    parseAttribute(&MToolbarItem::setShowOn,    element, ImTagShowOn,    params);
    parseAttribute(&MToolbarItem::setHideOn,    element, ImTagHideOn,    params);
    parseAttribute(&MToolbarItem::setAlignment, element, ImTagAlignment, params);
    parseAttribute(&MToolbarItem::setTextId,    element, ImTagTextId,    params);
    parseAttribute(&MToolbarItem::setIcon,      element, ImTagIcon,      params);
    parseAttribute(&MToolbarItem::setIconId,    element, ImTagIconId,    params);
    parseAttribute(&MToolbarItem::setToggle,    element, ImTagToggle,    params);
    parseAttribute(&MToolbarItem::setPressed,   element, ImTagPressed,   params);
    parseAttribute(&MToolbarItem::setPriority,  element, ImTagPriority,  params);

    if (element.hasAttribute(ImTagSize)) {
        bool ok;
        int size = element.attribute(ImTagSize).remove("%").toInt(&ok, 10);
        if (ok) {
            item->setSize(size);
        }
    }

    /*
     * We need to override button's actions here, so we will collect actions into temporary
     * object,...
     */
    QSharedPointer<MToolbarItem> tmpItem = QSharedPointer<MToolbarItem>(new MToolbarItem(*item));
    tmpItem->clearActions();
    params.currentItem = tmpItem;

    const MTBParseStructure parser(ImTagActions, &MToolbarData::parseTagActions);
    parseChildren(element, params, &parser);

    if (!tmpItem->actions().isEmpty()) {
        *item = *tmpItem; // ... and update shared object if needed
    }
}

void MToolbarData::parseTagLabel(const QDomElement &element, MTBParseParameters &params)
{
    const QString name = element.attribute(ImTagName);
    QSharedPointer<MToolbarItem> label = getOrCreateItemByName(name, MInputMethod::ItemLabel);

    if(label->type() != MInputMethod::ItemLabel) {
        return;
    }

    setCustom(true);
    params.currentRow->append(label);
    params.currentItem = label;

    parseAttribute(&MToolbarItem::setGroup, element, ImTagGroup, params);
    parseAttribute(&MToolbarItem::setPriority, element, ImTagPriority, params);
    parseAttribute(&MToolbarItem::setShowOn, element, ImTagShowOn, params);
    parseAttribute(&MToolbarItem::setHideOn, element, ImTagHideOn, params);
    parseAttribute(&MToolbarItem::setAlignment, element, ImTagAlignment, params);
    parseAttribute(&MToolbarItem::setText, element, ImTagText, params);
    parseAttribute(&MToolbarItem::setTextId, element, ImTagTextId, params);
}

void MToolbarData::parseTagActions(const QDomElement &element, MTBParseParameters &params)
{
    if (!params.currentItem || (params.currentItem->type() != MInputMethod::ItemButton)) {
        return;
    }

    const MTBParseStructure parsers[] = {
        MTBParseStructure(ImTagSendKeySequence, &MToolbarData::parseTagSendKeySequence),
        MTBParseStructure(ImTagSendString,      &MToolbarData::parseTagSendString),
        MTBParseStructure(ImTagSendCommand,     &MToolbarData::parseTagSendCommand),
        MTBParseStructure(ImTagCopy,            &MToolbarData::parseTagCopy),
        MTBParseStructure(ImTagPaste,           &MToolbarData::parseTagPaste),
        MTBParseStructure(ImTagShowGroup,       &MToolbarData::parseTagShowGroup),
        MTBParseStructure(ImTagHideGroup,       &MToolbarData::parseTagHideGroup),
        MTBParseStructure(ImTagCopyPaste,       &MToolbarData::parseTagCopyPaste),
        MTBParseStructure(ImTagClose,           &MToolbarData::parseTagClose),
    };

    parseChildren(element, params, parsers, sizeof(parsers) / sizeof(parsers[0]));
}

void MToolbarData::parseTagSendKeySequence(const QDomElement &element, MTBParseParameters &params)
{
    QSharedPointer<MToolbarItemAction> action(new MToolbarItemAction(MInputMethod::ActionSendKeySequence));
    action->setKeys(element.attribute(ImTagKeySequence));
    params.currentItem->append(action);
}

void MToolbarData::parseTagSendString(const QDomElement &element, MTBParseParameters &params)
{
    QSharedPointer<MToolbarItemAction> action(new MToolbarItemAction(MInputMethod::ActionSendString));
    action->setText(element.attribute(ImTagString));
    params.currentItem->append(action);
}

void MToolbarData::parseTagSendCommand(const QDomElement &element, MTBParseParameters &params)
{
    QSharedPointer<MToolbarItemAction> action(new MToolbarItemAction(MInputMethod::ActionSendCommand));
    action->setCommand(element.attribute(ImTagCommand));
    params.currentItem->append(action);
}

void MToolbarData::parseTagCopy(const QDomElement &element, MTBParseParameters &params)
{
    Q_UNUSED(element);
    QSharedPointer<MToolbarItemAction> action(new MToolbarItemAction(MInputMethod::ActionCopy));
    params.currentItem->append(action);
}

void MToolbarData::parseTagPaste(const QDomElement &element, MTBParseParameters &params)
{
    Q_UNUSED(element);
    QSharedPointer<MToolbarItemAction> action(new MToolbarItemAction(MInputMethod::ActionPaste));
    params.currentItem->append(action);
}

void MToolbarData::parseTagCopyPaste(const QDomElement &element, MTBParseParameters &params)
{
    Q_UNUSED(element);
    QSharedPointer<MToolbarItemAction> action(new MToolbarItemAction(MInputMethod::ActionCopyPaste));
    params.currentItem->append(action);
}

void MToolbarData::parseTagClose(const QDomElement &element, MTBParseParameters &params)
{
    Q_UNUSED(element);
    QSharedPointer<MToolbarItemAction> action(new MToolbarItemAction(MInputMethod::ActionClose));
    params.currentItem->append(action);
}

void MToolbarData::parseTagShowGroup(const QDomElement &element, MTBParseParameters &params)
{
    QSharedPointer<MToolbarItemAction> action(new MToolbarItemAction(MInputMethod::ActionShowGroup));
    action->setGroup(element.attribute(ImTagGroup));
    params.currentItem->append(action);
}

void MToolbarData::parseTagHideGroup(const QDomElement &element, MTBParseParameters &params)
{
    QSharedPointer<MToolbarItemAction> action(new MToolbarItemAction(MInputMethod::ActionHideGroup));
    action->setGroup(element.attribute(ImTagGroup));
    params.currentItem->append(action);
}

void MToolbarData::parseChildren(const QDomElement &element, MTBParseParameters &params,
                                 const MTBParseStructure *parserList, int parserCount)
{
    Q_ASSERT(parserCount > 0);

    for (QDomNode child = element.firstChild(); !child.isNull() && params.validTag;
            child = child.nextSibling()) {
        if (child.isElement()) {
            const QDomElement childElement = child.toElement();
            bool found = false;
            for (int i = 0; i < parserCount; ++i) {
                if (childElement.tagName() == parserList[i].tagName) {
                    (this->*(parserList[i].parser))(childElement, params);
                    found = true;
                    break;
                }
            }
            if (!found) {
                qWarning() << __PRETTY_FUNCTION__ << "Unexpected tag" << childElement.tagName() << "on line"
                           << childElement.lineNumber() << "column" << childElement.columnNumber()
                           << "in toolbar file" << params.fileName;
                params.validTag = false;
            }
        }
    }
}

