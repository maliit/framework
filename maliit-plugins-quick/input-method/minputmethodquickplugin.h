/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: Kimmo Surakka <kimmo.surakka@nokia.com>
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

#ifndef M_INPUT_METHOD_QUICK_PLUGIN
#define M_INPUT_METHOD_QUICK_PLUGIN

#include <maliit/plugins/inputmethodplugin.h>

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QSet>

class MInputMethodQuickPluginPrivate;

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
class MInputMethodQuickPlugin
    : public Maliit::Plugins::InputMethodPlugin
{
public:
    MInputMethodQuickPlugin();
    virtual ~MInputMethodQuickPlugin();

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
    //! \reimp_end

    //! Which QML file to load.
    virtual QString qmlFileName() const = 0;

private:
    Q_DISABLE_COPY(MInputMethodQuickPlugin);
    Q_DECLARE_PRIVATE(MInputMethodQuickPlugin);
    MInputMethodQuickPluginPrivate *const d_ptr;
};

Q_DECLARE_INTERFACE(MInputMethodQuickPlugin,
                    "com.meego.meegoimframework.MInputMethodQuickPlugin/1.1")

#endif // M_INPUT_METHOD_QUICK_PLUGIN
