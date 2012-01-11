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



#ifndef MIMPLUGINDESCRIPTION_H
#define MIMPLUGINDESCRIPTION_H

#include <QString>

class MImPluginDescriptionPrivate;
namespace Maliit {
namespace Plugins {
    class InputMethodPlugin;
}
}

/*! \ingroup maliitserver
 * \brief Provides information about loaded input method plugin
 */
class MImPluginDescription
{
public:
    //! Copy constructor.
    MImPluginDescription(const MImPluginDescription &other);

    //! Destructor.
    virtual ~MImPluginDescription();

    //! Assignment operator
    void operator=(const MImPluginDescription &other);

    //! Return plugin name.
    QString name() const;

    //! \brief Return true if this plugin is enabled by settings.
    bool enabled() const;

private:
    //! Constructor
    //! \param plugin Reference to loaded plugin.
    explicit MImPluginDescription(const Maliit::Plugins::InputMethodPlugin &plugin);

    //! Set enabled state to given value.
    void setEnabled(bool newEnabled);

    Q_DECLARE_PRIVATE(MImPluginDescription)

    MImPluginDescriptionPrivate * const d_ptr;

    friend class MIMPluginManagerPrivate;
};

#endif
