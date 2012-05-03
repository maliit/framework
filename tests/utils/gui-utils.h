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

#ifndef GUI_UTILS_H__
#define GUI_UTILS_H__

#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QInputContext>
#endif
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QWidget>

#include <tr1/functional>
#include <vector>

#include <minputmethodhost.h>
#include <minputcontextconnection.h>

#include <maliit/plugins/abstractsurface.h>
#include <maliit/plugins/abstractsurfacefactory.h>
#include <maliit/plugins/abstractwidgetssurface.h>

namespace MaliitTestUtils {

    class RemoteWindow : public QWidget
    {
    public:
        explicit RemoteWindow(QWidget *p = 0, Qt::WindowFlags f = 0);

        void paintEvent(QPaintEvent *);
    };

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
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
            this->push_back(transform(event));
            return true;
        }

    private:
        const TransformFunction transform;
    };
#endif

    using Maliit::Plugins::AbstractSurface;
    using Maliit::Plugins::AbstractWidgetSurface;
    using Maliit::Plugins::AbstractGraphicsViewSurface;

    class TestWidgetSurface : public AbstractWidgetSurface
    {
    public:
        TestWidgetSurface() {}

        void show() {}
        void hide() {}

        QSize size() const { return QSize(); }
        void setSize(const QSize&) {}

        QPoint relativePosition() const { return QPoint(); }
        void setRelativePosition(const QPoint &) {}

        QSharedPointer<AbstractSurface> parent() const { return QSharedPointer<AbstractSurface>(); }

        QPoint translateEventPosition(const QPoint &eventPosition, const QSharedPointer<AbstractSurface> & = QSharedPointer<AbstractSurface>()) const { return eventPosition; }

        QWidget *widget() const { return 0; }
    };

    class TestGraphicsViewSurface : public AbstractGraphicsViewSurface
    {
    public:
        TestGraphicsViewSurface()
            : mGraphicsView(new QGraphicsView(new QGraphicsScene))
            , mRootItem(new QGraphicsRectItem)
        {
            mGraphicsView->scene()->addItem(mRootItem.data());
        }

        void show() {}
        void hide() {}

        QSize size() const { return QSize(); }
        void setSize(const QSize&) {}

        QPoint relativePosition() const { return QPoint(); }
        void setRelativePosition(const QPoint &) {}

        QSharedPointer<AbstractSurface> parent() const { return QSharedPointer<AbstractSurface>(); }

        QPoint translateEventPosition(const QPoint &eventPosition, const QSharedPointer<AbstractSurface> & = QSharedPointer<AbstractSurface>()) const { return eventPosition; }

        QGraphicsScene *scene() const { return mGraphicsView->scene(); }
        QGraphicsView *view() const { return mGraphicsView.data(); }

        QGraphicsItem *root() const { return mRootItem.data(); }
        void clear() {}
    private:
        QScopedPointer<QGraphicsView> mGraphicsView;
        QScopedPointer<QGraphicsItem> mRootItem;
    };

    class TestSurfaceFactory : public Maliit::Plugins::AbstractSurfaceFactory
    {
    public:
        TestSurfaceFactory() {}

        bool supported(AbstractSurface::Options options) const { return options & (AbstractSurface::TypeGraphicsView | AbstractSurface::TypeWidget); }

        QSharedPointer<AbstractSurface> create(AbstractSurface::Options options, const QSharedPointer<AbstractSurface> &) {
            if (options & AbstractSurface::TypeGraphicsView)
                return QSharedPointer<AbstractSurface>(new TestGraphicsViewSurface);
            else if (options & AbstractSurface::TypeWidget)
                return QSharedPointer<AbstractSurface>(new TestWidgetSurface);
            else
                return QSharedPointer<AbstractSurface>();
        }
    };

    class TestInputMethodHost
        : public MInputMethodHost
    {
    public:
        QString lastCommit;
        int sendCommitCount;

        QString lastPreedit;
        int sendPreeditCount;

        TestInputMethodHost(MIndicatorServiceClient &client)
            : MInputMethodHost(std::tr1::shared_ptr<MInputContextConnection>(new MInputContextConnection), 0, client, new TestSurfaceFactory)
            , sendCommitCount(0)
            , sendPreeditCount(0)
        {}

        void sendCommitString(const QString &string,
                              int start, int length, int cursorPos)
        {
            lastCommit = string;
            ++sendCommitCount;
            MInputMethodHost::sendCommitString(string, start, length, cursorPos);
        }

        void sendPreeditString(const QString &string,
                               const QList<Maliit::PreeditTextFormat> &preeditFormats,
                               int start, int length, int cursorPos)
        {
            lastPreedit = string;
            ++sendPreeditCount;
            MInputMethodHost::sendPreeditString(string, preeditFormats, start, length, cursorPos);
        }
    };

}

// For cases where we need to run code _before_ QApplication is created
#define MALIIT_TESTUTILS_GUI_MAIN_WITH_SETUP(TestObject, setupFunc) \
int main(int argc, char *argv[]) \
{ \
    setupFunc();\
    QApplication app(argc, argv);\
    Q_UNUSED(app);\
    TestObject tc;\
    return QTest::qExec(&tc, argc, argv);\
}

#endif // UTILS_H__
