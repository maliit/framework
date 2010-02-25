#include "ut_duiinputcontext.h"
#include "qdbus_stub.h"

#include <QProcess>
#include <QStringList>
#include <QDebug>
#include <QInputContextFactory>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QPointer>
#include <QClipboard>

#include <DuiApplication>
#include <DuiTheme>
#include <DuiSceneManager>
#include <DuiSceneWindow>
#include <DuiScene>
#include <DuiNavigationBar>
#include <DuiComponentData>
#include "duipreeditinjectionevent.h"


namespace
{
    const QString DBUS_SERVICE_NAME("org.maemo.duiinputmethodserver1");
    const QString DBUS_PATH("/org/maemo/duiinputmethodserver1");
    const QString DBUS_INTERFACE("org.maemo.duiinputmethodserver1");

    const int WidgetStubCursorPosition(37);
    const QString WidgetStubSurroundingText("surrounding");
    const Qt::KeyboardModifier FnLevelModifier = Qt::GroupSwitchModifier;
    const Qt::Key FnLevelKey = Qt::Key_AltGr;
}

namespace
{
    QPointer<QWidget> gFocusedWidget = 0;
}

QWidget *QApplication::focusWidget()
{
    return gFocusedWidget;
}

///////////////////////////////////
//// DBus stub class implementation

void InputMethodServerDBusStub::RedirectedKeyParamsStruct::clear()
{
    keyType = 0;
    keyCode = 0;
    text.clear();
}

InputMethodServerDBusStub::InputMethodServerDBusStub(QObject *object)
{
    Q_UNUSED(object);
    resetCallCounts();
}


InputMethodServerDBusStub::~InputMethodServerDBusStub()
{
}


void InputMethodServerDBusStub::resetCallCounts()
{
    showOnFocusCallCount = 0;
    hideOnLostFocusCallCount = 0;
    mouseClickedOnPreeditCallCount = 0;
    setPreeditCallCount = 0;
    resetCallCount = 0;

    setContextObjectCallCount = 0;
    activateContextCallCount = 0;

    keyEventCallCount = 0;
    appOrientationChangedCount = 0;
    setCopyPasteButtonCallCount = 0;
    redirectKeyCallCount = 0;
}


int InputMethodServerDBusStub::showOnFocusCount()
{
    return showOnFocusCallCount;
}


int InputMethodServerDBusStub::hideOnLostFocusCount()
{
    return hideOnLostFocusCallCount;
}


int InputMethodServerDBusStub::mouseClickedOnPreeditCount()
{
    return mouseClickedOnPreeditCallCount;
}



int InputMethodServerDBusStub::setPreeditCount()
{
    return setPreeditCallCount;
}


int InputMethodServerDBusStub::resetCount()
{
    return resetCallCount;
}


int InputMethodServerDBusStub::setContextObjectCount()
{
    return setContextObjectCallCount;
}

int InputMethodServerDBusStub::activateContextCount()
{
    return activateContextCallCount;
}

int InputMethodServerDBusStub::keyEventCount()
{
    return keyEventCallCount;
}

int InputMethodServerDBusStub::orientationChangedCount()
{
    return appOrientationChangedCount;
}

int InputMethodServerDBusStub::setCopyPasteButtonCount()
{
    return setCopyPasteButtonCallCount;
}

QList<bool>& InputMethodServerDBusStub::setCopyPasteButtonParams()
{
    return setCopyPasteButtonCallParams;
}

int InputMethodServerDBusStub::redirectKeyCount()
{
    return redirectKeyCallCount;
}

InputMethodServerDBusStub::RedirectedKeyParamsStruct &InputMethodServerDBusStub::redirectKeyParams()
{
    return redirectKeyCallParams;
}

///////
void InputMethodServerDBusStub::showOnFocus()
{
    showOnFocusCallCount++;
}


void InputMethodServerDBusStub::hideOnLostFocus()
{
    hideOnLostFocusCallCount++;
}


void InputMethodServerDBusStub::mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect)
{
    Q_UNUSED(pos);
    Q_UNUSED(preeditRect);

    mouseClickedOnPreeditCallCount++;
}


void InputMethodServerDBusStub::setPreedit(const QString &text)
{
    Q_UNUSED(text)

    setPreeditCallCount++;
}


void InputMethodServerDBusStub::reset()
{
    resetCallCount++;
}


void InputMethodServerDBusStub::setContextObject(const QString &callbackObject)
{
    Q_UNUSED(callbackObject)

    setContextObjectCallCount++;
}


void InputMethodServerDBusStub::activateContext()
{
    activateContextCallCount++;
}

void InputMethodServerDBusStub::sendKeyEvent(const QKeyEvent &keyEvent)
{
    Q_UNUSED(keyEvent);
    keyEventCallCount++;
}

void InputMethodServerDBusStub::appOrientationChanged(int angle)
{
    Q_UNUSED(angle);
    appOrientationChangedCount++;
}

void InputMethodServerDBusStub::setCopyPasteButton(bool copyAvailable, bool pasteAvailable)
{
    ++setCopyPasteButtonCallCount;
    setCopyPasteButtonCallParams.append(copyAvailable);
    setCopyPasteButtonCallParams.append(pasteAvailable);
}

void InputMethodServerDBusStub::processKeyEvent(int keyType, int keyCode, int /* modifiers */,
                                                const QString &text, bool /* autoRepeat */,
                                                int /* count */, int /* nativeScanCode */)
{
    ++redirectKeyCallCount;
    redirectKeyCallParams.keyType = keyType;
    redirectKeyCallParams.keyCode = keyCode;
    redirectKeyCallParams.text = text;
}

//////////////
// widget stub

WidgetStub::WidgetStub(QObject *, bool enableVisualizationPriority)
    : m_lastKeyEvent(QKeyEvent::KeyPress, 0, 0),
      visualizationPriority(enableVisualizationPriority)
{
    setAttribute(Qt::WA_InputMethodEnabled);
    resetCounters();
}


WidgetStub::~WidgetStub()
{
}


QVariant WidgetStub::inputMethodQuery(Qt::InputMethodQuery query) const
{
    m_inputMethodQueryCount++;

    if (static_cast<int>(query) == Dui::VisualizationPriorityQuery) {
        return QVariant(visualizationPriority);
    } else if (static_cast<int>(query) == Dui::InputEnabledQuery) {
        return QVariant(true);
    } else if (query == Qt::ImSurroundingText) {
        return QVariant(WidgetStubSurroundingText);
    } else if (query == Qt::ImCursorPosition) {
        return QVariant(WidgetStubCursorPosition);
    } else if (query == Qt::ImCurrentSelection) {
        return QVariant(selectedText);
    }

    return QVariant();
}


void WidgetStub::inputMethodEvent(QInputMethodEvent *event)
{
    m_inputMethodEventCount++;
    m_lastImEvent = *event;
}

bool WidgetStub::event(QEvent *event)
{
    ++m_eventCount;

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *kevent = dynamic_cast<QKeyEvent *>(event);
        if (kevent)
            m_lastKeyEvent = *kevent;
    }

    return QWidget::event(event);
}


void WidgetStub::resetCounters()
{
    m_inputMethodEventCount = 0;
    m_inputMethodQueryCount = 0;
    m_eventCount = 0;
}


int WidgetStub::inputMethodQueryCount()
{
    return m_inputMethodQueryCount;
}


int WidgetStub::inputMethodEventCount()
{
    return m_inputMethodEventCount;
}

int WidgetStub::eventCount()
{
    return m_eventCount;
}

QInputMethodEvent WidgetStub::lastInputMethodEvent()
{
    return m_lastImEvent;
}
QKeyEvent WidgetStub::lastKeyEvent()
{
    return m_lastKeyEvent;
}

void WidgetStub::sendCopyAvailable(bool yes)
{
    emit copyAvailable(yes);
}


///////////////
// actual test


void Ut_DuiInputContext::initTestCase()
{
    // make sure we don't delete random crap on error situations
    m_subject = 0;
    m_stub = 0;
    app = 0;

    static char *argv[1] = { (char *) "ut_duiinputcontext" };
    static int argc = 1;

    QCoreApplication::setLibraryPaths(QStringList("./inputmethods"));
    DuiApplication::setLoadDuiInputContext(false);
    app = new DuiApplication(argc, argv);

    m_stub = new InputMethodServerDBusStub(this);

    qDBusInterfaceStub->target = m_stub;

    m_subject = new DuiInputContext;
    QVERIFY(m_subject != 0);
}


void Ut_DuiInputContext::cleanupTestCase()
{
    delete m_subject;
    delete m_stub;
    delete app;
}


void Ut_DuiInputContext::init()
{
    m_stub->resetCallCounts();
}

void Ut_DuiInputContext::cleanup()
{
}

void Ut_DuiInputContext::testAddCoverage()
{
    // Visit the less useful API here
    qDebug() << m_subject->identifierName();
    qDebug() << m_subject->isComposing();
    qDebug() << m_subject->language();
    m_subject->updateInputMethodArea(QList<QVariant>());
}

void Ut_DuiInputContext::testEvent()
{
    WidgetStub widget(0);

    gFocusedWidget = &widget;
    // test that input context accepts
    m_subject->setGlobalCorrectionEnabled(true);
    DuiPreeditInjectionEvent *injectionEvent = new DuiPreeditInjectionEvent("preedit");
    bool accepted = QCoreApplication::sendEvent(m_subject, injectionEvent);
    QVERIFY(accepted == true);

    waitAndProcessEvents(500);

    // This event claims to be pre-edit injection event but it's not.
    QEvent fakeInjectionEvent((QEvent::Type)DuiPreeditInjectionEvent::eventNumber());
    accepted = QCoreApplication::sendEvent(m_subject, &fakeInjectionEvent);
    QVERIFY(accepted == false);

    waitAndProcessEvents(500);

    QCOMPARE(m_stub->setPreeditCount(), 1);

    m_stub->resetCallCounts();
    m_subject->setGlobalCorrectionEnabled(false);
    injectionEvent->setAccepted(false);
    accepted = QCoreApplication::sendEvent(m_subject, injectionEvent);
    QVERIFY(accepted == false);

    waitAndProcessEvents(500);

    //won't call setPreedit if global error correction is off
    QCOMPARE(m_stub->setPreeditCount(), 0);

    delete injectionEvent;
    gFocusedWidget = 0;
}


void Ut_DuiInputContext::testReset()
{
    m_subject->reset();

    waitAndProcessEvents(500);

    QCOMPARE(m_stub->resetCount(), 1);
}


void Ut_DuiInputContext::testMouseHandler()
{
    // TODO: test mouse button press and maybe double click

    WidgetStub widget(0);
    int x = 4;
    // mouse left button release on (10, 10), no additional modifiers
    QPoint point(10, 10);
    QPoint globalPoint(20, 20);
    QMouseEvent event(QEvent::MouseButtonRelease, point, globalPoint, Qt::LeftButton,
                      Qt::LeftButton, Qt::NoModifier);

    m_subject->setFocusWidget(&widget);
    m_subject->mouseHandler(x, &event);

    waitAndProcessEvents(500);

    QCOMPARE(m_stub->mouseClickedOnPreeditCount(), 1);

    m_subject->setFocusWidget(0);
}


void Ut_DuiInputContext::testInputMethodHidden()
{
    // nothing yet
}


void Ut_DuiInputContext::testCommitString()
{
    WidgetStub widget(0);
    QString commitString("committed string");
    m_subject->setFocusWidget(&widget);
    gFocusedWidget = &widget;
    m_subject->commitString(commitString);

    waitAndProcessEvents(0);

    QCOMPARE(widget.inputMethodEventCount(), 1);
    QInputMethodEvent event = widget.lastInputMethodEvent();
    QVERIFY(event.preeditString() == ""); // check that event doesn't contain wrong information
    QCOMPARE(event.commitString(), commitString);

    gFocusedWidget = 0;
}


void Ut_DuiInputContext::testUpdatePreedit()
{
    WidgetStub widget(0);
    QList<QInputMethodEvent::Attribute> attributes;
    QInputMethodEvent event;
    QString updateString("preedit string");

    //test preedit with traditional style
    m_subject->setFocusWidget(&widget);
    gFocusedWidget = &widget;
    m_subject->updatePreedit(updateString, Dui::PreeditDefault);

    waitAndProcessEvents(0);

    QCOMPARE(widget.inputMethodEventCount(), 1);
    event = widget.lastInputMethodEvent();
    QVERIFY(event.preeditString() == updateString);
    QVERIFY(event.commitString() == "");
    attributes = event.attributes();
    QVERIFY(attributes.count() > 0);

    //test preedit with alternate style
    m_subject->updatePreedit(updateString, Dui::PreeditNoCandidates);

    waitAndProcessEvents(50);

    QCOMPARE(widget.inputMethodEventCount(), 2);
    event = widget.lastInputMethodEvent();
    QVERIFY(event.preeditString() == updateString);
    QVERIFY(event.commitString() == "");
    attributes = event.attributes();
    QVERIFY(attributes.count() > 0);

    gFocusedWidget = 0;
}

void Ut_DuiInputContext::testAppOrientationChanged()
{
    m_subject->notifyOrientationChange(Dui::Angle90);

    // Make sure DBus call gets through
    waitAndProcessEvents(300);

    // TODO: can not recieve signal, should check it
    QCOMPARE(m_stub->orientationChangedCount(), 1);
}

void Ut_DuiInputContext::testNonTextEntryWidget()
{

    int count = m_stub->hideOnLostFocusCount();
    gFocusedWidget = 0;

    QEvent close(QEvent::CloseSoftwareInputPanel);
    m_subject->filterEvent(&close);

    waitAndProcessEvents(1500);

    QCOMPARE(m_stub->hideOnLostFocusCount(), count + 1);
}

void Ut_DuiInputContext::testSendKeyEvent()
{
    QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
    m_stub->sendKeyEvent(keyEvent);
    QCOMPARE(m_stub->keyEventCount(), 1);

    WidgetStub widget(0);
    m_subject->setFocusWidget(&widget);
    m_stub->sendKeyEvent(keyEvent);
    QCOMPARE(m_stub->keyEventCount(), 2);

}

void Ut_DuiInputContext::testKeyEvent()
{
    QEvent::Type eventType = QEvent::KeyPress;
    int eventKey = Qt::Key_A;
    Qt::KeyboardModifiers eventModifiers = Qt::NoModifier;
    QString eventText('A');
    bool eventRepeat = false;
    unsigned short eventCount = 1;

    WidgetStub widget(0);
    gFocusedWidget = &widget;
    m_subject->setFocusWidget(&widget);
    widget.resetCounters();
    m_subject->keyEvent(eventType, eventKey, eventModifiers, eventText, eventRepeat, eventCount);
    QVERIFY(widget.eventCount() > 0);

    QKeyEvent expected(eventType, eventKey, eventModifiers, eventText, eventRepeat, eventCount);
    QKeyEvent result = widget.lastKeyEvent();

    QCOMPARE(result.type(), expected.type());
    QCOMPARE(result.key(), expected.key());
    QCOMPARE(result.modifiers(), expected.modifiers());
    QCOMPARE(result.text(), expected.text());
    QCOMPARE(result.count(), expected.count());

    m_subject->setFocusWidget(0);
    gFocusedWidget = 0;
}

void Ut_DuiInputContext::testFocusWidgetInfo()
{
    WidgetStub widget(0);
    DuiInputContext::WidgetInfo winfo;

    m_subject->setFocusWidget(0);
    gFocusedWidget = 0;
    winfo = m_subject->getFocusWidgetInfo();
    QVERIFY(!winfo.valid);

    widget.setInputMethodHints(Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    m_subject->setFocusWidget(&widget);
    gFocusedWidget = &widget;
    winfo = m_subject->getFocusWidgetInfo();
    QVERIFY(winfo.valid);
    QVERIFY(winfo.autoCapitalizationEnabled == false);
    QVERIFY(winfo.predictionEnabled == false);
    QVERIFY(winfo.contentType == Dui::FreeTextContentType);

    widget.setInputMethodHints(Qt::ImhNone);
    m_subject->setFocusWidget(&widget);
    gFocusedWidget = &widget;
    winfo = m_subject->getFocusWidgetInfo();
    QVERIFY(winfo.valid);
    QVERIFY(winfo.autoCapitalizationEnabled == true);
    QVERIFY(winfo.predictionEnabled == true);
    QVERIFY(winfo.contentType == Dui::FreeTextContentType);

    gFocusedWidget = 0;
}

void Ut_DuiInputContext::testSurroundingText()
{
    WidgetStub widget(0);
    QString text;
    int cursorPos;
    bool success;

    m_subject->setFocusWidget(NULL);
    gFocusedWidget = 0;
    success = m_subject->surroundingText(text, cursorPos);
    QVERIFY(!success);

    m_subject->setFocusWidget(&widget);
    gFocusedWidget = &widget;
    success = m_subject->surroundingText(text, cursorPos);
    QVERIFY(success);
    QCOMPARE(text, WidgetStubSurroundingText);
    QCOMPARE(cursorPos, WidgetStubCursorPosition);

    gFocusedWidget = 0;
}

void Ut_DuiInputContext::testHasSelection()
{
    WidgetStub widget(0);
    bool hasSelection = false;
    bool success;

    m_subject->setFocusWidget(NULL);
    gFocusedWidget = 0;
    hasSelection = m_subject->hasSelection(success);
    QVERIFY(!success);

    m_subject->setFocusWidget(&widget);
    gFocusedWidget = &widget;
    widget.selectedText = "Some text";
    hasSelection = m_subject->hasSelection(success);
    QVERIFY(success);
    QVERIFY(hasSelection);

    widget.selectedText = "";
    hasSelection = m_subject->hasSelection(success);
    QVERIFY(success);
    QVERIFY(!hasSelection);

    gFocusedWidget = 0;
}

void Ut_DuiInputContext::testCopyPasteButton()
{
    WidgetStub widget(0);
    QList<bool> &params = m_stub->setCopyPasteButtonParams();

    QApplication::clipboard()->clear();

    QTest::qWait(500); // just processing pending events is not robust.
    while (app->hasPendingEvents()) {
        app->processEvents();
    }

    int count = m_stub->setCopyPasteButtonCount();
    params.clear();
    qDebug() << "No focused widget";
    gFocusedWidget = 0;
    m_subject->setFocusWidget(0);
    QTest::qWait(500); // just processing pending events is not robust.
    while (app->hasPendingEvents()) {
        app->processEvents();
    }
    ++count;
    QCOMPARE(m_stub->setCopyPasteButtonCount(), count);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.takeFirst(), false);
    QCOMPARE(params.takeFirst(), false);

    qDebug() << "Update button state";
    m_subject->manageCopyPasteButton(false);
    QTest::qWait(500); // just processing pending events is not robust.
    while (app->hasPendingEvents()) {
        app->processEvents();
    }
    ++count;
    QCOMPARE(m_stub->setCopyPasteButtonCount(), count);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.takeFirst(), false);
    QCOMPARE(params.takeFirst(), false);

    qDebug() << "Set widget without selection";
    gFocusedWidget = &widget;
    m_subject->setFocusWidget(&widget);
    QTest::qWait(500); // just processing pending events is not robust.
    while (app->hasPendingEvents()) {
        app->processEvents();
    }
    ++count;
    QCOMPARE(m_stub->setCopyPasteButtonCount(), count);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.takeFirst(), false);
    QCOMPARE(params.takeFirst(), false);

    qDebug() << "Update button state with empty selection";
    widget.sendCopyAvailable(false);
    QTest::qWait(500); // just processing pending events is not robust.
    while (app->hasPendingEvents()) {
        app->processEvents();
    }
    ++count;
    QCOMPARE(m_stub->setCopyPasteButtonCount(), count);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.takeFirst(), false);
    QCOMPARE(params.takeFirst(), false);

    widget.selectedText = "Some text";
    widget.sendCopyAvailable(true);
    qDebug() << "Text was selected";
    QTest::qWait(500); // just processing pending events is not robust.
    while (app->hasPendingEvents()) {
        app->processEvents();
    }
    ++count;
    QVERIFY(m_stub->setCopyPasteButtonCount() == count);
    QVERIFY(params.count() == 2);
    QCOMPARE(params.takeFirst(), true);
    QCOMPARE(params.takeFirst(), false);

    qDebug() << "Restore initial state";
    gFocusedWidget = 0;
    m_subject->setFocusWidget(0);
    QTest::qWait(500); // just processing pending events is not robust.
    while (app->hasPendingEvents()) {
        app->processEvents();
    }
    ++count;
    QVERIFY(m_stub->setCopyPasteButtonCount() == count);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.takeFirst(), false);
    QCOMPARE(params.takeFirst(), false);
}

void Ut_DuiInputContext::testSetRedirectKeys()
{
    // no redirection should happen
    int count = m_stub->redirectKeyCount();
    QKeyEvent request(QEvent::KeyPress, Qt::Key_Shift, Qt::ShiftModifier, "");
    bool ret = m_subject->filterEvent(&request);
    QVERIFY(!ret);
    QCOMPARE(m_stub->redirectKeyCount(), count);

    // redirection should happen
    m_subject->setRedirectKeys(true);
    InputMethodServerDBusStub::RedirectedKeyParamsStruct &params = m_stub->redirectKeyParams();
    params.clear();
    ret = m_subject->filterEvent(&request);
    QVERIFY(ret);
    QCOMPARE(m_stub->redirectKeyCount(), count + 1);
    QCOMPARE(params.keyType, static_cast<int>(QEvent::KeyPress));
    QCOMPARE(params.keyCode, static_cast<int>(Qt::Key_Shift));
    QCOMPARE(params.text, QString(""));

    // no redirection should happen
    m_subject->setRedirectKeys(false);
    ret = m_subject->filterEvent(&request);
    QVERIFY(!ret);
    QCOMPARE(m_stub->redirectKeyCount(), count + 1);
}

void Ut_DuiInputContext::waitAndProcessEvents(int waitTime)
{
    QTest::qWait(waitTime);
    while (app->hasPendingEvents()) {
        app->processEvents();
    }
}

QTEST_APPLESS_MAIN(Ut_DuiInputContext)

