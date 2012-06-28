/* * This file is part of maliit-framework *
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

#include "maliitquickpluginfactory.h"
#include "minputmethodquickplugin.h"

class MyPlugin
    : public MInputMethodQuickPlugin
{
    Q_DISABLE_COPY(MyPlugin)

private:
    const QString m_filename;

public:
    explicit MyPlugin(const QString &filename)
        : MInputMethodQuickPlugin()
        , m_filename(filename)
    {}

    virtual QString qmlFileName() const
    {
        return m_filename;
    }

    virtual QString name() const
    {
        static QFileInfo info(m_filename);
        return info.baseName();
    }
};

MaliitQuickPluginFactory::MaliitQuickPluginFactory(QObject *parent)
    : QObject(parent)
    , MImAbstractPluginFactory()
{}

MaliitQuickPluginFactory::~MaliitQuickPluginFactory()
{}

QString MaliitQuickPluginFactory::fileExtension() const
{
    return "qml";
}

Maliit::Plugins::InputMethodPlugin * MaliitQuickPluginFactory::create(const QString &file) const
{
    return new MyPlugin(file);
}

Q_EXPORT_PLUGIN2(MaliitQuickPluginFactory, MaliitQuickPluginFactory)
