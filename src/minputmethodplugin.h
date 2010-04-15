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

#ifndef MINPUTMETHODPLUGIN_H
#define MINPUTMETHODPLUGIN_H

#include <QStringList>
#include <QtPlugin>

#include "minputmethodbase.h"

class MInputContextConnection;
class MInputMethodBase;


/*!
 * \brief MInputMethodPlugin is an interface class for all input method plugins.
 *
 * To create a virtual keyboard / input method plugin, re-implement the virtual functions and
 * instantiate the input method implementation in the createInputMethod() method.
 * Make sure your plugin links against the m im framework library as well.
 */
class MInputMethodPlugin
{
public:
    /*! \brief Implement this function to return the identifier for this input method.
     */
    virtual QString name() const = 0;

    /*! \brief Implement this function to return the languages supported for this virtual keyboard.
     */
    virtual QStringList languages() const = 0;

    /*! \brief Creates and returns the MInputMethodBase object for
     * this plugin. This function will be only called once and the allocated
     * resources will be owned by the input method server.
     */
    virtual MInputMethodBase *createInputMethod(MInputContextConnection *icConnection) = 0;
};


Q_DECLARE_INTERFACE(MInputMethodPlugin,
                    "com.nokia.maemo.MInputMethodPlugin/1.0")

#endif
