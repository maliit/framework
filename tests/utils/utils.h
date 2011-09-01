/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef UTILS_H__
#define UTILS_H__

#include <QInputContext>
#include <QWidget>

#include <tr1/functional>
#include <vector>

namespace MaliitTestUtils {

    bool isTestingInSandbox();
    QString getTestPluginPath();
    void waitForSignal(const QObject* object, const char* signal, int timeout);

    class RemoteWindow : public QWidget
    {
    public:
        explicit RemoteWindow(QWidget *p = 0, Qt::WindowFlags f = 0);

        void paintEvent(QPaintEvent *);
    };

    template <typename T>
    class EventSpyInputContext : public QInputContext, public std::vector<T>
    {
    public:
        typedef std::tr1::function<T (const QEvent *event)> TransformFunction;

        EventSpyInputContext(TransformFunction newTransform) : transform(newTransform) {}

        virtual QString identifierName() { return QString::fromLatin1("EventSpyInputContext"); }
        virtual bool isComposing() const { return false; }
        virtual QString language() { return QString::fromLatin1("EN"); }
        virtual void reset() {}

    protected:
        virtual bool filterEvent(const QEvent *event) {
            push_back(transform(event));
            return true;
        }

    private:
        const TransformFunction transform;
    };
}

#endif // UTILS_H__
