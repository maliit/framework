/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include <maliit/plugins/abstractinputmethodhost.h>
#include <maliit/plugins/subviewdescription.h>

class MAbstractInputMethodHostPrivate
{
public:
    MAbstractInputMethodHostPrivate();
    ~MAbstractInputMethodHostPrivate();
};


MAbstractInputMethodHostPrivate::MAbstractInputMethodHostPrivate()
{
}

MAbstractInputMethodHostPrivate::~MAbstractInputMethodHostPrivate()
{
}


MAbstractInputMethodHost::MAbstractInputMethodHost(QObject *parent)
    : QObject(parent),
      d(new MAbstractInputMethodHostPrivate)
{
    // nothing
}

MAbstractInputMethodHost::~MAbstractInputMethodHost()
{
    delete d;
}

bool MAbstractInputMethodHost::hiddenText(bool &valid)
{
    valid = false;
    return false;
}

QPixmap MAbstractInputMethodHost::background() const
{
    return QPixmap();
}

QList<MImSubViewDescription>
MAbstractInputMethodHost::surroundingSubViewDescriptions(Maliit::HandlerState /*state*/) const
{
    return QList<MImSubViewDescription>();
}

void MAbstractInputMethodHost::setLanguage(const QString &/*language*/)
{
}
