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

#ifndef MINDICATORSERVICECLIENT_H
#define MINDICATORSERVICECLIENT_H

#include <maliit/namespace.h>

/*! \internal
 * \ingroup maliitserver
 * \brief Provides interface for setting the system keyboard indicator state
 */
class MIndicatorServiceClient
{
public:
    //! \brief Constructs a MIndicatorServiceClient which does not do anything
    MIndicatorServiceClient();
    virtual ~MIndicatorServiceClient();

    //! \brief Set the indicator to \a mode
    virtual void setInputModeIndicator(Maliit::InputModeIndicator mode);

private:
    Q_DISABLE_COPY(MIndicatorServiceClient)
};
//! \internal_end

#endif // MINDICATORSERVICECLIENT_H
