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

#include "mimserveroptions.h"

#include <stdio.h>
#include <string.h>

#include <QtGlobal>
#include <QList>
#include <QExplicitlySharedDataPointer>
#include <QSharedData>

namespace {

    struct CommandLineParameter {
        const char * name;
        const char * description;
    };

    CommandLineParameter AvailableConnectionParameters[] = {
        { "-allow-anonymous",   "Allow anonymous/unauthenticated use of DBus interface"},
        { "-override-address",  "Override the DBus peer-to-peer address for input-context"}
    };

    struct IgnoredParameter {
        const char * name;
        bool hasArgument;
    } IgnoredParameters [] = {
        // Following parameters are used by Qt, so we should not show error messages or help about it
        { "-style",          true },
        { "-session",        true },
        { "-graphicssystem", true },
        { "-testability",    true },
        { "-qdevel",         false },
        { "-reverse",        false },
        { "-stylesheet",     false },
        { "-widgetcount",    false },
        { "-qdebug",         false },
        { "-software",       false },
        { "-qws",            false },
        { "-sync",           false }
    };

    const char * programName = "meego-im-uiserver";

    /*! \internal
     * \brief Base class for parsers.
     */
    struct MImServerOptionsParserBase : public QSharedData
    {
        MImServerOptionsParserBase(void *options);

        virtual ~MImServerOptionsParserBase();

        //! Result of parameter parsing
        enum ParsingResult {
            //! Parameter was not recognized
            Invalid = -1,
            //! Parameter was recognized
            Ok,
        };

        /*!
         * \brief Parse one \a parameter from command line.
         * \param[in] next Next parameter if it exists, otherwise 0.
         * It should be used if current \a parameter should have argument.
         * \param[out] argumentCount Amount of arguments which follow \a paramter.
         * Current implementation assumes that it should 0 or 1.
         * \return Ok if \a parameter is recognized by this parser
         * and should not be passed to other parsers, otherwise Invalid.
         */
        virtual ParsingResult parseParameter(const char * parameter,
                                             const char * next,
                                             int *argumentCount) = 0;

        /*!
         * \brief Print options which could be recognized by this parser.
         */
        virtual void printAvailableOptions(const char *format) = 0;

        void *options() const;

    private:
        void *serverOptions;
    };

    typedef QExplicitlySharedDataPointer<MImServerOptionsParserBase> ParserBasePtr;
    QList<ParserBasePtr> parsers;

    //! Unregister parser associated with given \a options.
    void unregisterParser(void *options)
    {
        QList<ParserBasePtr>::iterator iterator = parsers.begin();
        while (iterator != parsers.end()) {
            if ((*iterator)->options() == options) {
                iterator = parsers.erase(iterator);
            } else {
                ++iterator;
            }
        }
    }

     //! \brief Parser of common command line parameters
    struct MImServerCommonOptionsParser : public MImServerOptionsParserBase
    {
        MImServerCommonOptionsParser(MImServerCommonOptions *options);

        //! \reimp
        virtual ParsingResult parseParameter(const char * parameter,
                                             const char * next,
                                             int *argumentCount);
        virtual void printAvailableOptions(const char *format);
        //! \reimp_end

    private:
        MImServerCommonOptions *storage;
    };

    /*!
     * \brief Parser of command line parameters for the server<->input context connection
     */
    struct MImServerConnectionOptionsParser : public MImServerOptionsParserBase
    {
        /*! \brief Construct new instance.
         * Object should be created when application starts and should stay alive until moment
         * when application will exit, so it is recommnded to create it in main().
         * \note It does not makes sense tp create more than one object of this class.
         */
        MImServerConnectionOptionsParser(MImServerConnectionOptions *options);

        //! \reimp
        virtual ParsingResult parseParameter(const char * parameter,
                                             const char * next,
                                             int *argumentCount);
        virtual void printAvailableOptions(const char *format);
        //! \reimp_end

    private:
        MImServerConnectionOptions *storage;
    };

    struct MImServerIgnoredOptions
    {
        MImServerIgnoredOptions();
        ~MImServerIgnoredOptions();
    };

    struct MImServerIgnoredOptionsParser : public MImServerOptionsParserBase
    {
        MImServerIgnoredOptionsParser(MImServerIgnoredOptions *options);

        //! \reimp
        virtual ParsingResult parseParameter(const char * parameter,
                                             const char * next,
                                             int *argumentCount);
        virtual void printAvailableOptions(const char *format);
        //! \reimp_end
    };

    //! \internal_end

    // Always instantiate error suppressor, so main() should not worry about it
    MImServerIgnoredOptions IgnoredOptions;

    const char * const HelpFormat = "%-30s\t%s\n";
} // namespace

///////////////
// parser interface
MImServerOptionsParserBase::MImServerOptionsParserBase(void *options)
    : serverOptions(options)
{
}

MImServerOptionsParserBase::~MImServerOptionsParserBase()
{
}

void* MImServerOptionsParserBase::options() const
{
    return serverOptions;
}

bool parseCommandLine(int argc, const char * const * argv)
{
    bool allRecognized = true;

    if (argc > 0) {
        programName = argv[0];
    }

    for (int n = 1; n < argc; ++n) {
        const char * const parameter = argv[n];
        const char * const next = (n < argc - 1) ? argv[n + 1] : 0;
        MImServerOptionsParserBase::ParsingResult parsingResult = MImServerOptionsParserBase::Invalid;

        Q_FOREACH (const ParserBasePtr &base, parsers) {
            int skippedParameters = 0;
            parsingResult = base->parseParameter(parameter, next, &skippedParameters);
            if (parsingResult == MImServerOptionsParserBase::Ok) {
                n += skippedParameters;
                break;
            }
        }

        if (parsingResult == MImServerOptionsParserBase::Invalid) {
            fprintf(stderr, "Invalid parameter '%s'\n", argv[n]);
            allRecognized = false;
        }
    }

    return allRecognized;
}

void printHelpMessage()
{
    fprintf(stderr, "\nUsage: %s [options]\n", programName);
    fprintf(stderr, "Available options:\n");

    Q_FOREACH (const ParserBasePtr &base, parsers) {
        base->printAvailableOptions(HelpFormat);
    }

    // parsers will not be used anymore,
    // therefore we can destroy them all
    parsers.clear();
}

///////////////
// parser for ignored options
MImServerIgnoredOptionsParser::MImServerIgnoredOptionsParser(MImServerIgnoredOptions *options)
    : MImServerOptionsParserBase(options)
{
}

MImServerOptionsParserBase::ParsingResult
MImServerIgnoredOptionsParser::parseParameter(const char *parameter,
                                              const char *,
                                              int *argumentCount)
{
    const int count = sizeof(IgnoredParameters) / sizeof(IgnoredParameters[0]);
    ParsingResult result = Invalid;

    *argumentCount = 0;

    for (int i = 0; i < count; ++i) {
        if (!strcmp(parameter, IgnoredParameters[i].name)) {
            result = Ok;
            *argumentCount = IgnoredParameters[i].hasArgument ? 1 : 0;
            break;
        }
    }
    return result;
}

void MImServerIgnoredOptionsParser::printAvailableOptions(const char *)
{
    // nothing to print
}

MImServerIgnoredOptions::MImServerIgnoredOptions()
{
    const ParserBasePtr p(new MImServerIgnoredOptionsParser(this));
    parsers.append(p);
}

MImServerIgnoredOptions::~MImServerIgnoredOptions()
{
    unregisterParser(this);
}

///////////////
// parser for common options
MImServerCommonOptionsParser::MImServerCommonOptionsParser(MImServerCommonOptions *options)
    : MImServerOptionsParserBase(options),
    storage(options)
{
}

MImServerOptionsParserBase::ParsingResult
MImServerCommonOptionsParser::parseParameter(const char *parameter,
                                             const char *,
                                             int *argumentCount)
{
    *argumentCount = 0;

    if (!strcmp("-help", parameter)) {
        storage->showHelp = true;

        return Ok;
    }

    return Invalid;
}

void MImServerCommonOptionsParser::printAvailableOptions(const char *format)
{
    fprintf(stderr, format, "-help", "Show usage information");
}

MImServerCommonOptions::MImServerCommonOptions()
    : showHelp(false)
{
    const ParserBasePtr p(new MImServerCommonOptionsParser(this));
    parsers.append(p);
}

MImServerCommonOptions::~MImServerCommonOptions()
{
    unregisterParser(this);
}

MImServerConnectionOptionsParser::MImServerConnectionOptionsParser(MImServerConnectionOptions *options)
    : MImServerOptionsParserBase(options)
    , storage(options)
{
}

MImServerOptionsParserBase::ParsingResult
MImServerConnectionOptionsParser::parseParameter(const char *parameter,
                                                 const char *next,
                                                 int *argumentCount)
{
    const int count = sizeof(AvailableConnectionParameters) / sizeof(AvailableConnectionParameters[0]);
    ParsingResult result = Invalid;

    for (int i = 0; i < count; ++i) {
        const char * const availableParameter = AvailableConnectionParameters[i].name;

        if (!strcmp(parameter, availableParameter)) {

            result = Ok;

            if (!strcmp(parameter, "-allow-anonymous")) {
                storage->allowAnonymous = true;
                *argumentCount = 0;
            } else if (!strcmp(parameter, "-override-address")) {
                if (next) {
                    storage->overriddenAddress = QString::fromUtf8(next);
                    *argumentCount = 1;
                } else {
                    fprintf(stderr, "ERROR: No argument passed to -override-address\n");
                    *argumentCount = 0;
                }
            } else {
                fprintf(stderr, "ERROR: connection option %s declared but unhandled\n", parameter);
            }

            break;
        }
    }

    return result;
}

void MImServerConnectionOptionsParser::printAvailableOptions(const char *format)
{
    const int count = sizeof(AvailableConnectionParameters) / sizeof(AvailableConnectionParameters[0]);

    for (int i = 0; i < count; ++i) {
        if (AvailableConnectionParameters[i].description) {
            fprintf(stderr, format, AvailableConnectionParameters[i].name,
                    AvailableConnectionParameters[i].description);
        }
    }
}
MImServerConnectionOptions::MImServerConnectionOptions()
    : allowAnonymous(false)
{
    const ParserBasePtr p(new MImServerConnectionOptionsParser(this));
    parsers.append(p);
}

MImServerConnectionOptions::~MImServerConnectionOptions()
{
    unregisterParser(this);
}
