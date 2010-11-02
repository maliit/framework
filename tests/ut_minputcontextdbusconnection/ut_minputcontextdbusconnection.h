#ifndef UT_MINPUTCONTEXTDBUSCONNECTION_H
#define UT_MINPUTCONTEXTDBUSCONNECTION_H

#include <QtTest/QtTest>
#include <QObject>
#include <QDBusAbstractAdaptor>
#include <QDBusInterface>

#include "minputcontextdbusconnection.h"
#include "mabstractinputmethod.h"


class MApplication;
class QGraphicsScene;
class MInputContext;

// input method base stub that dbus connection calls
class TargetStub: public MAbstractInputMethod
{
public:
    TargetStub(MInputContextConnection *icConnection, QObject *parent = 0);
    virtual ~TargetStub();

    virtual void show();
    virtual void hide();
    virtual void setPreedit(const QString &preeditString);
    virtual void reset();
    virtual void mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect);
    virtual void update();
    virtual void visualizationPriorityChanged(bool enabled);
    virtual void appOrientationChanged(int angle);
    virtual void processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                 Qt::KeyboardModifiers modifiers, const QString &text,
                                 bool autoRepeat, int count, quint32 nativeScanCode,
                                 quint32 nativeModifiers);

    void resetCallCounts();
    int showCallCount();
    int hideCallCount();
    int setPreeditCallCount();
    int resetCallCount();
    int mouseClickedOnPreeditCallCount();
    int updateCallCount();
    int visualizationPriorityChangedCallCount();
    int appOrientationChangedCallCount();
    int processKeyEventCallCount();

private:
    int m_showCallCount;
    int m_hideCallCount;
    int m_setPreeditCallCount;
    int m_resetCallCount;
    int m_mouseClickedOnPreeditCallCount;
    int m_updateCallCount;
    int m_visualizationPriorityChangedCallCount;
    int m_appOrientationChangedCallCount;
    int m_processKeyEventCallCount;
};

// test class
class Ut_MInputContextDBusConnection : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    // communication interface to input context
    void testNoReplyDBusCalls();

    // communication interface exposed to dbus
    void testShowOnFocus();
    void testHideOnLostFocus();
    void testMouseClickedOnPreedit();
    void testSetPreedit();
    void testReset();
    void testAppOrientationChanged();
    void testProcessKeyEvent();

private:
    void handleMessages();

private:
    MApplication *app;
    MInputContextDBusConnection *m_subject;
    TargetStub *m_inputMethod;
    QGraphicsScene *m_scene;
    QDBusInterface *m_clientInterface;
    MInputContext *m_inputContext;
};



#endif
