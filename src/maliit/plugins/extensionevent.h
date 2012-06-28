/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MIMEXTENSIONEVENT_H
#define MIMEXTENSIONEVENT_H

#include <QObject>

class MImExtensionEventPrivate;

/*! \ingroup pluginapi
 * \brief Base class for extending signaling from input method system to
 * plugins.
 *
 * This event can be subclassed for future additions.
 *
 * \sa MAbstractInputMethod::imExtensionEvent(MImExtensionEvent *event)
 */
class MImExtensionEvent
{
public:
    //! Defines valid types for input method extension event
    enum Type {
        None,
        Update
    };

    explicit MImExtensionEvent(Type type);
    virtual ~MImExtensionEvent();

    //! Returns the type of the event
    Type type() const;

protected:
    MImExtensionEvent(MImExtensionEventPrivate *dd,
                      Type type);

    MImExtensionEventPrivate * const d_ptr;

private:
    Q_DISABLE_COPY(MImExtensionEvent)
    Q_DECLARE_PRIVATE(MImExtensionEvent)
};

#endif
