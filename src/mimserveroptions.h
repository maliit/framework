/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef MIMSERVEROPTIONS_H
#define MIMSERVEROPTIONS_H

#include <QtGlobal>
#include <QString>

//! \internal

/*! \brief Parse command line arguments and update values of members
 * of this class.
 * \param argc Amount of arguments.
 * \param argv Command line parameters.
 * \return true if all parameters were recognized by parser
 */
bool parseCommandLine(int argc, const char * const * argv);

//! Display help message to the screen.
void printHelpMessage();

struct MImServerConnectionOptions
{
public:
    MImServerConnectionOptions();
    ~MImServerConnectionOptions();

    //! Contains true if user asks for help or provided incorrect parameter
    bool allowAnonymous;
    QString overriddenAddress;
};


struct MImServerCommonOptions
{
public:
    MImServerCommonOptions();

    ~MImServerCommonOptions();

    //! Contains true if user asks for help or provided incorrect parameter
    bool showHelp;
};

//! \internal_end

#endif
