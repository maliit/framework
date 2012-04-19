#ifndef UT_MINPUTCONTEXT_H
#define UT_MINPUTCONTEXT_H

#include <memory>
#include <QPointer>
#include <QtTest/QtTest>
#include <QObject>

#include <minputcontext.h>
#include <mimserverconnection.h>

class InputMethodServerTestConnection: public MImServerConnection
{
    Q_OBJECT

public:
    struct RedirectedKeyParamsStruct {
        int keyType;
        int keyCode;
        QString text;
        void clear();
    };

    InputMethodServerTestConnection(QObject *parent = 0);
    virtual ~InputMethodServerTestConnection();

    // method calls are counted, this resets counters
    void resetCallCounts();
    void emitConnected();

    // returns counter for specific methods
    int showInputMethodCount();
    int hideInputMethodCount();
    int mouseClickedOnPreeditCount();
    int setPreeditCount();
    int resetCount();

    int activateContextCount();
    int keyEventCount();
    int orientationChangedCount();

    const QString &lastPreedit();

    int setCopyPasteStateCount();
    QList<bool> &setCopyPasteStateParams();

    int redirectKeyCount();
    RedirectedKeyParamsStruct &redirectKeyParams();

public Q_SLOTS:
    //! reimpl
    virtual void showInputMethod();
    virtual void hideInputMethod();
    virtual void mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect);
    virtual void setPreedit(const QString &text, int cursorPos);
    virtual void reset(bool);

    virtual void activateContext();

    virtual void appOrientationChanged(int angle);

    virtual void setCopyPasteState(bool copyAvailable, bool pasteAvailable);

    virtual void processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                 Qt::KeyboardModifiers modifiers,
                                 const QString &text, bool autoRepeat, int count,
                                 quint32 nativeScanCode, quint32 nativeModifiers,
                                 unsigned long time);
    //! reimpl end

private:
    int showInputMethodCallCount;
    int hideInputMethodCallCount;
    int mouseClickedOnPreeditCallCount;
    int setPreeditCallCount;
    int resetCallCount;

    int activateContextCallCount;
    int appOrientationChangedCount;

    int setCopyPasteStateCallCount;
    QList<bool> setCopyPasteStateCallParams;
    int redirectKeyCallCount;
    RedirectedKeyParamsStruct redirectKeyCallParams;
    QString lastPreeditString;
    int lastPreeditCursorPos;
};


class WidgetStub: public QWidget
{
    Q_OBJECT

public:
    WidgetStub(QObject *, bool enableVisualizationPriority = false);
    virtual ~WidgetStub();

    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;
    virtual void inputMethodEvent(QInputMethodEvent *event);
    virtual bool event(QEvent *event);

    void resetCounters();
    int inputMethodQueryCount();
    int inputMethodEventCount();
    int eventCount();
    QInputMethodEvent lastInputMethodEvent();
    QKeyEvent lastKeyEvent();

    void sendCopyAvailable(bool yes);

    QString selectedText;

Q_SIGNALS:
    void copyAvailable(bool);

private:
    mutable int m_inputMethodQueryCount;
    mutable int m_inputMethodEventCount;
    mutable QInputMethodEvent m_lastImEvent;
    int m_eventCount;
    QKeyEvent m_lastKeyEvent;
    bool visualizationPriority;
};



class Ut_MInputContext : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    // input context interface
    void testAddCoverage();
    void testEvent();
    void testReset();
    void testMouseHandler();
    void testKeyEvent();
    //void testSetFocusWidget();

    // test outbound input method server communication
    void testInputMethodHidden();
    void testCommitString_data();
    void testCommitString();
    void testUpdatePreedit();
    void testAppOrientationChanged();

    // test inbound input method server communication
    void testImAreaChangePropagation();
    void testImInitiatedHideUnfocusesWidget();
    void testImCommitStringInjection();
    void testImSelectionInjection();
    void testImGetSelection();

    void testNonTextEntryWidget();

    // FIXME: do separate tests for qgraphics based widget stub?
    // FIXME: test non-input enabled widgets?

    //test copy/paste stuff
    void testCopyPasteState();

    void testSetRedirectKeys();

    void testInvalidScene();

    void testPropertyNameNormalization_data();
    void testPropertyNameNormalization();

private:
    void waitAndProcessEvents(int waitTime);

private:
    QPointer<MInputContext> m_subject;
    QSharedPointer<InputMethodServerTestConnection> m_connection;
};



#endif
