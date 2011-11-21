/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MIMXEXTENSION_H
#define MIMXEXTENSION_H

#include <QObject>

/*! \internal
 * \ingroup maliitserver
 * \brief A helper class to check if X extensions are supported, and query
 * their event and error bases.
 */
class MImXExtension
{
public:
    //! Construct and MImXExtension object for extension \a name
    explicit MImXExtension(const char *name);

    //! Check wether the X extension is available and the version is recent enough.
    bool supported(int required_major = 0, int required_minor = 0) const;

    //! Returns the major code of the extension
    int majorCode() const { return major_code; }
    //! Returns the event base of the extension
    int eventBase() const { return event_base; }
    //! Returns the error base of the extension
    int errorBase() const { return error_base; }

protected:
    virtual bool queryVersion(int *major, int *minor) const = 0;

private:
    bool available;
    int major_code;
    int event_base;
    int error_base;

    Q_DISABLE_COPY(MImXExtension)
};

//! \brief A helper class for the X composite extension.
class MImXCompositeExtension : public MImXExtension
{
public:
    explicit MImXCompositeExtension();

protected:
    virtual bool queryVersion(int *major, int *minor) const;

private:
    Q_DISABLE_COPY(MImXCompositeExtension)
};

//! \brief A helper class for the X damage extension.
class MImXDamageExtension : public MImXExtension
{
public:
    explicit MImXDamageExtension();

protected:
    virtual bool queryVersion(int *major, int *minor) const;

private:
    Q_DISABLE_COPY(MImXDamageExtension)
};
//! \end_internal

#endif // MIMXEXTENSION_H
