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

#ifndef MINPUTMETHODPLUGIN_H
#define MINPUTMETHODPLUGIN_H

#include <QStringList>
#include <QtPlugin>
#include <QSet>

#include <maliit/namespace.h>

class MAbstractInputMethod;
class MAbstractInputMethodHost;

namespace Maliit {
namespace Plugins {

/*! \ingroup pluginapi
 * \brief An interface class for all input method plugins.
 *
 * To create a virtual keyboard / input method plugin, re-implement the virtual
 * functions and instantiate the input method implementation in the
 * createInputMethod() method. Make sure your plugin links against the m im
 * framework library as well.
 */
class InputMethodPlugin
{
public:
    /*! \brief Implement this function to return the identifier for this input method.
     */
    virtual QString name() const = 0;

    /*! \brief Creates and returns the MAbstractInputMethod object for
     * this plugin. This function will be only called once and the allocated
     * resources will be owned by the input method server.
     */
    virtual MAbstractInputMethod *createInputMethod(MAbstractInputMethodHost *host) = 0;

    /*!
     * \brief Returns set of states which could be handled by this plugin.
     *
     * WARNING: If result is empty then this plugin will not be loaded
     * during startup.
     */
    virtual QSet<Maliit::HandlerState> supportedStates() const = 0;
};

}
}

Q_DECLARE_INTERFACE(Maliit::Plugins::InputMethodPlugin,
                    "org.maliit.plugins.InputMethodPlugin/1.1")

#endif
