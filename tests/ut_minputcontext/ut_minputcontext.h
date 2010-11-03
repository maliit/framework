#ifndef UT_MINPUTCONTEXT_H
#define UT_MINPUTCONTEXT_H

#include <MApplication>

#include <memory>
#include <QPointer>
#include <QtTest/QtTest>
#include <QObject>

#include <minputcontext.h>

class InputMethodServerDBusStub: public QObject
{
    Q_OBJECT

public:
    struct RedirectedKeyParamsStruct {
        int keyType;
        int keyCode;
        QString text;
        void clear();
    };

    InputMethodServerDBusStub(QObject *);
    virtual ~InputMethodServerDBusStub();

    // method calls are counted, this resets counters
    void resetCallCounts();

    // returns counter for specific methods
    int showInputMethodCount();
    int hideInputMethodCount();
    int mouseClickedOnPreeditCount();
    int setPreeditCount();
    int resetCount();

    int setContextObjectCount();
    int activateContextCount();
    int keyEventCount();
    int orientationChangedCount();

    int setCopyPasteStateCount();
    QList<bool> &setCopyPasteStateParams();

    int redirectKeyCount();
    RedirectedKeyParamsStruct &redirectKeyParams();

public slots:
    void showInputMethod();
    void hideInputMethod();
    void mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect);
    void setPreedit(const QString &text, int cursorPos);
    void reset();

    void setContextObject(const QString &callbackObject);
    void activateContext();
    void sendKeyEvent(const QKeyEvent &keyEvent,
                      MInputMethod::EventRequestType requestType);

    void appOrientationChanged(int angle);

    void setCopyPasteState(bool copyAvailable, bool pasteAvailable);

    void processKeyEvent(int keyType, int keyCode, int modifiers, const QString &text,
                         bool autoRepeat, int count,
                         unsigned int nativeScanCode, unsigned int nativeModifiers);

private:
    int showInputMethodCallCount;
    int hideInputMethodCallCount;
    int mouseClickedOnPreeditCallCount;
    int setPreeditCallCount;
    int resetCallCount;

    int setContextObjectCallCount;
    int activateContextCallCount;
    int keyEventCallCount;
    int appOrientationChangedCount;

    int setCopyPasteStateCallCount;
    QList<bool> setCopyPasteStateCallParams;
    int redirectKeyCallCount;
    RedirectedKeyParamsStruct redirectKeyCallParams;
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

signals:
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

private slots:
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

    // methods for input method server connection
    void testInputMethodHidden();
    void testCommitString();
    void testUpdatePreedit();
    void testAppOrientationChanged();

    void testNonTextEntryWidget();
    void testSendKeyEvent();

    // FIXME: do separate tests for qgraphics based widget stub?
    // FIXME: test non-input enabled widgets?

    //test copy/paste stuff
    void testCopyPasteState();

    void testSetRedirectKeys();

    void testInvalidScene();

private:
    void waitAndProcessEvents(int waitTime);

private:
    std::auto_ptr<MApplication> app;
    QPointer<MInputContext> m_subject;
    QPointer<InputMethodServerDBusStub> m_stub;
};



#endif
