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

#ifndef MINPUTMETHODSETTINGSBASE_H
#define MINPUTMETHODSETTINGSBASE_H

#include <QString>

class QGraphicsWidget;

/*!
 * \brief MInputMethodSettingsBase is a base class for input method settings.
 *
 * MInputMethodSettingsBase defines the setting of a input method plugin. contentWidget()
 * holds its content.
 * Each input method plugin can implement its own setting by inheriting this class.
 *
 */

class MInputMethodSettingsBase
{
public:
    /*!
     *\brief Returns the title of the setting.
     * The title should be already translated according current display language.
     */
    virtual QString title() = 0;

    /*!
     *\brief Returns the icon of the setting.
     * It is the absolute file name for the icon
     */
    virtual QString icon() = 0;

    /*!
     *\brief Creates and returns the content widget of the setting.
     * \param parent pointer of QGraphicsWidget, it is set to the parent for content widget.
     *
     * WARNING: The return pointer of QGraphicsWidget is owned by the caller, and \parent is set
     * as its parent. It could be manually deleted outside or together with \a parent.
     */
    virtual QGraphicsWidget *createContentWidget(QGraphicsWidget *parent = 0) = 0;

};

#endif
