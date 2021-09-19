/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef MALIIT_INPUT_METHOD_QUICK_PLUGIN_H
#define MALIIT_INPUT_METHOD_QUICK_PLUGIN_H

#include <maliit/plugins/inputmethodplugin.h>

#include <QScopedPointer>
#include <QSet>
#include <QString>
#include <QStringList>

namespace Maliit
{

class AbstractPlatform;
class InputMethodQuickPluginPrivate;

//! \brief Creates an input method plugin that allows to use QML.
//!
//! To create a QML-based virtual keyboard or input method plugin, just drop the
//! QML file in plugin directory. If there are more QML files for the plugin
//! then make sure to put them into some subdirectory or otherwise Maliit server
//! will try to load them as well as separate plugins.
//! The QML components can communicate with the framework through the
//! Maliit::InputMethodQuick context.
//! If the provided Maliit::InputMethodQuick class is not sufficient, then
//! reimplement Maliit::InputMethodQuickPlugin::createInputMethodSettings as
//! well and create a custom MAbstractInputMethod instance there.
class InputMethodQuickPlugin
    : public Maliit::Plugins::InputMethodPlugin
{
public:
    InputMethodQuickPlugin(const QString &filename,
                           const QSharedPointer<Maliit::AbstractPlatform> &platform);
    virtual ~InputMethodQuickPlugin();

    //! \reimp
    virtual MAbstractInputMethod *createInputMethod(MAbstractInputMethodHost *host);
    virtual QSet<Maliit::HandlerState> supportedStates() const;
    virtual QString name() const;
    //! \reimp_end

private:
    Q_DISABLE_COPY(InputMethodQuickPlugin)
    Q_DECLARE_PRIVATE(InputMethodQuickPlugin)

    const QScopedPointer<InputMethodQuickPluginPrivate> d_ptr;
};

} // namespace Maliit

#endif // MALIIT_INPUT_METHOD_QUICK_PLUGIN_H
