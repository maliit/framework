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

#ifndef MIMSERVEROPTIONS_H
#define MIMSERVEROPTIONS_H

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

/*! \ingroup maliitserver
 * \brief Container for command line parameters which are applicable if
 * input method server works with X11.
 */
struct MImServerXOptions
{
public:
    /*! \brief Construct new instance.
     * Object should be created when application starts and should stay alive until moment
     * when application will exit, so it is recommnded to create it in main().
     * \note It does not makes sense tp create more than one object of this class.
     */
    MImServerXOptions();

    ~MImServerXOptions();

    //! Contains true if self composition is enabled
    bool selfComposited;

    //! Contains true if manual redirection is enabled
    bool manualRedirection;

    //! Contains true if window manages is bypassed
    bool bypassWMHint;

    //! Contains true if show() should be always called for server window
    //! regardless of current state of server and remote windows
    bool unconditionalShow;
};

//! \internal_end

#endif
