#ifndef UT_DUIINPUTCONTEXT_H
#define UT_DUIINPUTCONTEXT_H

#include <QtTest/QtTest>
#include <QObject>
#include <QDBusAbstractAdaptor>

#include <duiinputcontext.h>

class DuiApplication;

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
    int showOnFocusCount();
    int hideOnLostFocusCount();
    int mouseClickedOnPreeditCount();
    int setPreeditCount();
    int resetCount();

    int setContextObjectCount();
    int activateContextCount();
    int keyEventCount();
    int orientationChangedCount();

    int setCopyPasteButtonCount();
    QList<bool> &setCopyPasteButtonParams();

    int redirectKeyCount();
    RedirectedKeyParamsStruct &redirectKeyParams();

public slots:
    void showOnFocus();
    void hideOnLostFocus();
    void mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect);
    void setPreedit(const QString &text);
    void reset();

    void setContextObject(const QString &callbackObject);
    void activateContext();
    void sendKeyEvent(const QKeyEvent &keyEvent);

    void appOrientationChanged(int angle);

    void setCopyPasteButton(bool copyAvailable, bool pasteAvailable);

    void redirectKey(int keyType, int keyCode, const QString &text);

private:
    int showOnFocusCallCount;
    int hideOnLostFocusCallCount;
    int mouseClickedOnPreeditCallCount;
    int setPreeditCallCount;
    int resetCallCount;

    int setContextObjectCallCount;
    int activateContextCallCount;
    int keyEventCallCount;
    int appOrientationChangedCount;

    int setCopyPasteButtonCallCount;
    QList<bool> setCopyPasteButtonCallParams;
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



class Ut_DuiInputContext : public QObject
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
    void testFocusWidgetInfo();
    void testSurroundingText();
    void testHasSelection();
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
    void testCopyPasteButton();

    void testRedirectKeys();
    void testsetNextKeyRedirected();

private:
    void waitAndProcessEvents(int waitTime);

private:
    DuiApplication *app;
    DuiInputContext *m_subject;
    InputMethodServerDBusStub *m_stub;
};



#endif
