/* * This file is part of dui-im-framework *
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

#ifndef DUIPREEDITSTYLE_H
#define DUIPREEDITSTYLE_H

#include <DuiStyle>
#include <QColor>
#include <QString>
#include <QTextCharFormat>

/*!
 * \brief This class provides access to style attributes for DuiInputContext
 */
class DuiPreeditStyle : public DuiStyle
{
    Q_OBJECT

    DUI_STYLE(DuiPreeditStyle)

public:
    DUI_STYLE_ATTRIBUTE(QTextCharFormat::UnderlineStyle, underline, Underline)
    DUI_STYLE_ATTRIBUTE(QColor, fontColor, FontColor)
    DUI_STYLE_ATTRIBUTE(QColor, backgroundColor, BackgroundColor)
};

class DuiPreeditStyleContainer : public DuiStyleContainer
{
    DUI_STYLE_CONTAINER(DuiPreeditStyle)
    DUI_STYLE_MODE(NoCandidates)
};

#endif
