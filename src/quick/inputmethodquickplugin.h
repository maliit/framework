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
//! To create a QML-based virtual keyboard or input method plugin, derive from
//! this class and reimplement MInputMethodPlugin::name() and
//! MInputMethodQuickPlugin::qmlFileName().
//! The QML components can communicate with the framework through the
//! MInputMethodQuick context.
//! If the provided MInputMethodQuick class is not sufficient, then reimplement
//! MInputMethodQuickPlugin::createInputMethodSettings as well and create a
//! custom MAbstractInputMethod instance there.
class InputMethodQuickPlugin
    : public Maliit::Plugins::InputMethodPlugin
{
public:
    InputMethodQuickPlugin(const QString &filename,
                           const QSharedPointer<Maliit::AbstractPlatform> &platform);
    virtual ~InputMethodQuickPlugin();

    // FIXME: Add getter as vfunc with next API break and remove both static functions.
    //! Add list of import paths that will be used by the internal QML engine of this plugin.
    //! For this function to take any effect, it needs to be called before the
    //! reimplementation of createInputMethod calls MInputMethodQuickPlugin::createInputMethod.
    static void setQmlImportPaths(const QStringList &paths);

    //! \internal
    //! Used to read the current list of import paths.
    static QStringList qmlImportPaths();
    //! \internal_end

    //! \reimp
    virtual MAbstractInputMethod *createInputMethod(MAbstractInputMethodHost *host);
    virtual QSet<Maliit::HandlerState> supportedStates() const;
    virtual QString name() const;
    //! \reimp_end

private:
    Q_DISABLE_COPY(InputMethodQuickPlugin);
    Q_DECLARE_PRIVATE(InputMethodQuickPlugin);
    const QScopedPointer<InputMethodQuickPluginPrivate> d_ptr;
};

} // namespace Maliit

#endif // MALIIT_INPUT_METHOD_QUICK_PLUGIN_H
