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

#ifndef MPREEDITSTYLE_H
#define MPREEDITSTYLE_H

#include <MStyle>
#include <QColor>
#include <QString>
#include <QTextCharFormat>

/*!
 * \brief This class provides access to style attributes for MInputContext
 */
class MPreeditStyle : public MStyle
{
    Q_OBJECT

    M_STYLE(MPreeditStyle)

public:
    M_STYLE_ATTRIBUTE(QTextCharFormat::UnderlineStyle, underline, Underline)
    M_STYLE_ATTRIBUTE(QColor, fontColor, FontColor)
    M_STYLE_ATTRIBUTE(QColor, backgroundColor, BackgroundColor)
};

class MPreeditStyleContainer : public MStyleContainer
{
    M_STYLE_CONTAINER(MPreeditStyle)
    M_STYLE_MODE(NoCandidates)
};

#endif
