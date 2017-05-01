/* 
 * Copyright (c) 2017 Jan Arne Petersen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 *
 */

#ifndef MALIIT_STANDALONEINPUTMETHOD_H
#define MALIIT_STANDALONEINPUTMETHOD_H

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>

#include <memory>

class MAbstractInputMethod;
class MInputContextConnection;

namespace Maliit
{
class AbstractPlatform;
class StandaloneInputMethodHost;
class WindowGroup;

namespace Plugins
{
class InputMethodPlugin;
}

class StandaloneInputMethod: public QObject
{
Q_OBJECT

public:
    StandaloneInputMethod(Maliit::Plugins::InputMethodPlugin *plugin);
    ~StandaloneInputMethod();

private:
    std::unique_ptr<MInputContextConnection> mConnection;
    QSharedPointer<AbstractPlatform> mPlatform; // TODO use std::unique_ptr instead
    std::unique_ptr<WindowGroup> mWindowGroup;
    std::unique_ptr<StandaloneInputMethodHost> mInputMethodHost;
    std::unique_ptr<MAbstractInputMethod> mInputMethod;
};

std::unique_ptr<MInputContextConnection> createConnection();

}

#endif //MALIIT_STANDALONEINPUTMETHOD_H
