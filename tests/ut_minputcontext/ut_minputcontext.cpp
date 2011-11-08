#include "ut_minputcontext.h"

#include <QStringList>
#include <QDebug>
#include <QPointer>
#include <QClipboard>
#include <QGraphicsView>
#include <QLineEdit>

#include <maliit/namespace.h>
#include <maliit/inputmethod.h>
#include <maliit/preeditinjectionevent.h>

#ifdef HAVE_MEEGOTOUCH
#include <mpreeditinjectionevent.h>
#endif

namespace
{
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

/* */
void InputMethodServerTestConnection::RedirectedKeyParamsStruct::clear()
{
    keyType = 0;
    keyCode = 0;
    text.clear();
}

InputMethodServerTestConnection::InputMethodServerTestConnection(QObject *object)
{
    Q_UNUSED(object);
    resetCallCounts();
}

InputMethodServerTestConnection::~InputMethodServerTestConnection()
{
}

void InputMethodServerTestConnection::emitConnected()
{
    Q_EMIT connected();
}

void InputMethodServerTestConnection::resetCallCounts()
{
    showInputMethodCallCount = 0;
    hideInputMethodCallCount = 0;
    mouseClickedOnPreeditCallCount = 0;
    setPreeditCallCount = 0;
    resetCallCount = 0;

    activateContextCallCount = 0;

    appOrientationChangedCount = 0;
    setCopyPasteStateCallCount = 0;
    redirectKeyCallCount = 0;
}


int InputMethodServerTestConnection::showInputMethodCount()
{
    return showInputMethodCallCount;
}


int InputMethodServerTestConnection::hideInputMethodCount()
{
    return hideInputMethodCallCount;
}

int InputMethodServerTestConnection::mouseClickedOnPreeditCount()
{
    return mouseClickedOnPreeditCallCount;
}

int InputMethodServerTestConnection::setPreeditCount()
{
    return setPreeditCallCount;
}

int InputMethodServerTestConnection::resetCount()
{
    return resetCallCount;
}

int InputMethodServerTestConnection::activateContextCount()
{
    return activateContextCallCount;
}

int InputMethodServerTestConnection::orientationChangedCount()
{
    return appOrientationChangedCount;
}

int InputMethodServerTestConnection::setCopyPasteStateCount()
{
    return setCopyPasteStateCallCount;
}

QList<bool>& InputMethodServerTestConnection::setCopyPasteStateParams()
{
    return setCopyPasteStateCallParams;
}

int InputMethodServerTestConnection::redirectKeyCount()
{
    return redirectKeyCallCount;
}

InputMethodServerTestConnection::RedirectedKeyParamsStruct &InputMethodServerTestConnection::redirectKeyParams()
{
    return redirectKeyCallParams;
}

///////
void InputMethodServerTestConnection::showInputMethod()
{
    MImServerConnection::showInputMethod();
    showInputMethodCallCount++;
}


void InputMethodServerTestConnection::hideInputMethod()
{
    MImServerConnection::hideInputMethod();
    hideInputMethodCallCount++;
}


void InputMethodServerTestConnection::mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect)
{
    MImServerConnection::mouseClickedOnPreedit(pos, preeditRect);
    mouseClickedOnPreeditCallCount++;
}


void InputMethodServerTestConnection::setPreedit(const QString &text, int cursorPos)
{
    MImServerConnection::setPreedit(text, cursorPos);
    lastPreeditString = text;
    lastPreeditCursorPos = cursorPos;
    setPreeditCallCount++;
}

const QString &InputMethodServerTestConnection::lastPreedit()
{
    return lastPreeditString;
}

void InputMethodServerTestConnection::reset(bool requireSyncronization)
{
    MImServerConnection::reset(requireSyncronization);
    resetCallCount++;
}

void InputMethodServerTestConnection::activateContext()
{
    MImServerConnection::activateContext();
    activateContextCallCount++;
}

void InputMethodServerTestConnection::appOrientationChanged(int angle)
{
    MImServerConnection::appOrientationChanged(angle);
    appOrientationChangedCount++;
}

void InputMethodServerTestConnection::setCopyPasteState(bool copyAvailable, bool pasteAvailable)
{
    MImServerConnection::setCopyPasteState(copyAvailable, pasteAvailable);
    ++setCopyPasteStateCallCount;
    setCopyPasteStateCallParams.append(copyAvailable);
    setCopyPasteStateCallParams.append(pasteAvailable);
}

void InputMethodServerTestConnection::processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                                Qt::KeyboardModifiers modifiers,
                                                const QString &text, bool autoRepeat, int count,
                                                quint32 nativeScanCode, quint32 nativeModifiers,
                                                unsigned long time)
{
    MImServerConnection::processKeyEvent(keyType, keyCode, modifiers,
                                         text, autoRepeat, count,
                                         nativeScanCode, nativeModifiers, time);
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
    setFocusPolicy(Qt::StrongFocus);
    resetCounters();
}


WidgetStub::~WidgetStub()
{
}


QVariant WidgetStub::inputMethodQuery(Qt::InputMethodQuery query) const
{
    m_inputMethodQueryCount++;

    if (query == Qt::ImSurroundingText) {
        return QVariant(WidgetStubSurroundingText);
    } else if (static_cast<int>(query) == Maliit::VisualizationPriorityQuery) {
        return QVariant(visualizationPriority);
    } else if (query == Qt::ImCursorPosition
               || query == Qt::ImAnchorPosition) {
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
    Q_EMIT copyAvailable(yes);
}


///////////////
// actual test


void Ut_MInputContext::initTestCase()
{
    static char *argv[2] = { (char *) "ut_minputcontext",
                             (char *) "-software"};
    static int argc = 2;

    // QInputContext::setFocusWidget seems to require a valid X11 window.
    // However, the test crashes with MeeGo GS. That's why we set the graphics
    // system manually. Should be removed once NB#220339 [1] gets fixed.
    // [1] "QWidget::create crashes in window-less application"
    QApplication::setGraphicsSystem("raster");

    QCoreApplication::setLibraryPaths(QStringList("./inputmethods"));
    app.reset(new QApplication(argc, argv));

    m_connection = new InputMethodServerTestConnection(0);
    m_subject = new MInputContext(m_connection, 0);
    m_connection->emitConnected();
    QVERIFY(m_subject != 0);

    // Overly cautious sanity check, but we do use native C API in
    // MInputContext and friends:
    QCOMPARE(TRUE, true);
    QCOMPARE(FALSE, false);
}


void Ut_MInputContext::cleanupTestCase()
{
    delete m_subject;
    delete m_connection;
}


void Ut_MInputContext::init()
{
    m_connection->resetCallCounts();
}

void Ut_MInputContext::cleanup()
{
}

void Ut_MInputContext::testAddCoverage()
{
    // Visit the less useful API here
    qDebug() << m_subject->identifierName();
    qDebug() << m_subject->isComposing();
    qDebug() << m_subject->language();
    m_subject->updateInputMethodArea(QRect());
}

void Ut_MInputContext::testEvent()
{
    WidgetStub widget(0);

    gFocusedWidget = &widget;
    // test that input context accepts
    m_subject->setGlobalCorrectionEnabled(true);
    Maliit::PreeditInjectionEvent *injectionEvent = new Maliit::PreeditInjectionEvent("preedit");
    bool accepted = QCoreApplication::sendEvent(m_subject, injectionEvent);
    QVERIFY(accepted == true);

    waitAndProcessEvents(500);

    QCOMPARE(m_connection->setPreeditCount(), 1);

    m_connection->resetCallCounts();
    m_subject->setGlobalCorrectionEnabled(false);
    injectionEvent->setAccepted(false);
    accepted = QCoreApplication::sendEvent(m_subject, injectionEvent);
    QVERIFY(accepted == false);

    waitAndProcessEvents(500);

    //won't call setPreedit if global error correction is off
    QCOMPARE(m_connection->setPreeditCount(), 0);

    delete injectionEvent;
    gFocusedWidget = 0;
}


void Ut_MInputContext::testReset()
{
    WidgetStub widget(0);
    QInputMethodEvent event;
    QString preeditString("");
    QList<MInputMethod::PreeditTextFormat> preeditFormats;
    MInputMethod::PreeditTextFormat preeditFormat;

    preeditFormat.start = 0;
    preeditFormat.preeditFace = MInputMethod::PreeditDefault;
    m_subject->setFocusWidget(&widget);
    gFocusedWidget = &widget;

    // Reset with empty pre-edit
    preeditFormat.length = preeditString.length();
    preeditFormats << preeditFormat;
    m_subject->updatePreedit(preeditString, preeditFormats, -1);
    widget.resetCounters();

    m_subject->reset();

    waitAndProcessEvents(500);

    QCOMPARE(m_connection->resetCount(), 1);
    QCOMPARE(widget.inputMethodEventCount(), 0);

    m_connection->resetCallCounts();
    widget.resetCounters();
    preeditFormats.clear();

    // Reset with non-empty pre-edit
    preeditString = "some string";
    preeditFormat.length = preeditString.length();
    preeditFormats << preeditFormat;
    m_subject->updatePreedit(preeditString, preeditFormats, -1);
    widget.resetCounters();

    m_subject->reset();

    waitAndProcessEvents(500);

    QCOMPARE(m_connection->resetCount(), 1);
    QCOMPARE(widget.inputMethodEventCount(), 1);
    event = widget.lastInputMethodEvent();
    QCOMPARE(event.preeditString(), QString(""));
    QCOMPARE(event.commitString(),  preeditString);
}


void Ut_MInputContext::testMouseHandler()
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
    // need to make sure there is really preedit on top of which mouse events happen
    QList<MInputMethod::PreeditTextFormat> preeditFormats;
    m_subject->updatePreedit("aaaaaaaaa", preeditFormats);
    m_subject->mouseHandler(x, &event);

    waitAndProcessEvents(500);

    QCOMPARE(m_connection->mouseClickedOnPreeditCount(), 1);

    m_subject->setFocusWidget(0);
}


void Ut_MInputContext::testInputMethodHidden()
{
    // nothing yet
}

void Ut_MInputContext::testCommitString_data()
{
    QTest::addColumn<int>("cursorPosition");
    QTest::addColumn<int>("replacementStart");

    for (int cursorPos = -3; cursorPos <= 3; ++cursorPos) {
        for (int replacementStart = -3; replacementStart <= 3; ++replacementStart) {
            QString testName(QString("cursor pos %1, replacement start %2").arg(cursorPos).arg(replacementStart));
            QTest::newRow(testName.toAscii().data())
                << cursorPos << replacementStart;
        }
    }
}

void Ut_MInputContext::testCommitString()
{
    QFETCH(int, cursorPosition);
    QFETCH(int, replacementStart);

    WidgetStub widget(0);
    QString commitString("committed string");
    m_subject->setFocusWidget(&widget);
    gFocusedWidget = &widget;
    m_subject->commitString(commitString,
                            replacementStart,
                            0, cursorPosition);

    waitAndProcessEvents(0);

    QCOMPARE(widget.inputMethodEventCount(), 1);
    QInputMethodEvent event = widget.lastInputMethodEvent();
    QVERIFY(event.preeditString() == ""); // check that event doesn't contain wrong information
    QCOMPARE(event.commitString(), commitString);

    Q_FOREACH (QInputMethodEvent::Attribute attribute, event.attributes()) {
        if (attribute.type == QInputMethodEvent::Selection) {
            // For Selection type, Attribute::start means absolute cursor
            // position after commit string has been committed.
            QCOMPARE(attribute.start,
                     WidgetStubCursorPosition
                     + replacementStart
                     + cursorPosition);
        }
    }

    gFocusedWidget = 0;
}


void Ut_MInputContext::testUpdatePreedit()
{
    WidgetStub widget(0);
    QList<QInputMethodEvent::Attribute> attributes;
    QInputMethodEvent event;
    QString updateString("preedit string");

    //test preedit with traditional style
    m_subject->setFocusWidget(&widget);
    gFocusedWidget = &widget;
    QList<MInputMethod::PreeditTextFormat> preeditFormats;
    MInputMethod::PreeditTextFormat preeditFormat(0, updateString.length(),
                                                  MInputMethod::PreeditDefault);
    preeditFormats << preeditFormat;
    m_subject->updatePreedit(updateString, preeditFormats, -1);

    waitAndProcessEvents(0);

    QCOMPARE(widget.inputMethodEventCount(), 1);
    event = widget.lastInputMethodEvent();
    QVERIFY(event.preeditString() == updateString);
    QVERIFY(event.commitString() == "");
    attributes = event.attributes();
    QVERIFY(attributes.count() > 0);

    //test preedit with alternate style
    preeditFormats.clear();
    preeditFormat.start = 0;
    preeditFormat.length = updateString.length();
    preeditFormat.preeditFace = MInputMethod::PreeditNoCandidates;
    preeditFormats << preeditFormat;
    m_subject->updatePreedit(updateString, preeditFormats, -1);

    waitAndProcessEvents(50);

    QCOMPARE(widget.inputMethodEventCount(), 2);
    event = widget.lastInputMethodEvent();
    QVERIFY(event.preeditString() == updateString);
    QVERIFY(event.commitString() == "");
    attributes = event.attributes();
    QVERIFY(attributes.count() > 0);

    gFocusedWidget = 0;
}

void Ut_MInputContext::testAppOrientationChanged()
{
    m_subject->notifyOrientationChanged(Maliit::Angle90);

    waitAndProcessEvents(300);

    // TODO: can not recieve signal, should check it
    QCOMPARE(m_connection->orientationChangedCount(), 1);
}

void Ut_MInputContext::testNonTextEntryWidget()
{

    int count = m_connection->hideInputMethodCount();
    gFocusedWidget = 0;

    QEvent close(QEvent::CloseSoftwareInputPanel);
    m_subject->filterEvent(&close);

    waitAndProcessEvents(1500);

    QCOMPARE(m_connection->hideInputMethodCount(), count + 1);
}

void Ut_MInputContext::testKeyEvent()
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

void Ut_MInputContext::testCopyPasteState()
{
    WidgetStub widget(0);
    QList<bool> &params = m_connection->setCopyPasteStateParams();

    QApplication::clipboard()->clear();

    waitAndProcessEvents(100);

    int count = m_connection->setCopyPasteStateCount();
    params.clear();
    qDebug() << "No focused widget";
    gFocusedWidget = 0;
    m_subject->setFocusWidget(0);
    waitAndProcessEvents(100);
    ++count;
    QCOMPARE(m_connection->setCopyPasteStateCount(), count);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.takeFirst(), false);
    QCOMPARE(params.takeFirst(), false);

    qDebug() << "Set widget without selection";
    gFocusedWidget = &widget;
    m_subject->setFocusWidget(&widget);
    waitAndProcessEvents(100);
    ++count;
    QCOMPARE(m_connection->setCopyPasteStateCount(), count);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.takeFirst(), false);
    QCOMPARE(params.takeFirst(), false);

    qDebug() << "Update button state with empty selection";
    widget.sendCopyAvailable(false);
    waitAndProcessEvents(100);
    QCOMPARE(m_connection->setCopyPasteStateCount(), count);

    qDebug() << "Update button state with something selected";
    widget.selectedText = "Some text";
    widget.sendCopyAvailable(true);
    qDebug() << "Text was selected";
    waitAndProcessEvents(100);
    ++count;
    QVERIFY(m_connection->setCopyPasteStateCount() == count);
    QVERIFY(params.count() == 2);
    QCOMPARE(params.takeFirst(), true);
    QCOMPARE(params.takeFirst(), false);

    qDebug() << "Set some text to clipboard (simulate paste)";
    QApplication::clipboard()->setText("Some text");
    waitAndProcessEvents(100);
    ++count;
    QCOMPARE(m_connection->setCopyPasteStateCount(), count);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.takeFirst(), true);
    QCOMPARE(params.takeFirst(), true);

    qDebug() << "Update button state with text unselected";
    widget.sendCopyAvailable(false);
    waitAndProcessEvents(100);
    ++count;
    QCOMPARE(m_connection->setCopyPasteStateCount(), count);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.takeFirst(), false);
    QCOMPARE(params.takeFirst(), true);

    qDebug() << "Clear clipboard";
    QApplication::clipboard()->clear();
    waitAndProcessEvents(100);
    ++count;
    QCOMPARE(m_connection->setCopyPasteStateCount(), count);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.takeFirst(), false);
    QCOMPARE(params.takeFirst(), false);

    qDebug() << "Restore initial state";
    gFocusedWidget = 0;
    m_subject->setFocusWidget(0);
    waitAndProcessEvents(100);
    ++count;
    QVERIFY(m_connection->setCopyPasteStateCount() == count);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.takeFirst(), false);
    QCOMPARE(params.takeFirst(), false);

    qDebug() << "Set some text to clipboard (simulate paste)";
    QApplication::clipboard()->setText("Some text");
    waitAndProcessEvents(100);
    QVERIFY(m_connection->setCopyPasteStateCount() == count);

    qDebug() << "Clear clipboard";
    QApplication::clipboard()->clear();
    waitAndProcessEvents(100);
    QVERIFY(m_connection->setCopyPasteStateCount() == count);
}

void Ut_MInputContext::testSetRedirectKeys()
{
    WidgetStub widget(0);
    m_subject->setFocusWidget(&widget);
    // no redirection should happen
    int count = m_connection->redirectKeyCount();
    QKeyEvent request(QEvent::KeyPress, Qt::Key_Shift, Qt::ShiftModifier, "");
    bool ret = m_subject->filterEvent(&request);
    QVERIFY(!ret);
    QCOMPARE(m_connection->redirectKeyCount(), count);

    // redirection should happen
    m_subject->setRedirectKeys(true);
    InputMethodServerTestConnection::RedirectedKeyParamsStruct &params = m_connection->redirectKeyParams();
    params.clear();
    ret = m_subject->filterEvent(&request);
    QVERIFY(ret);
    QCOMPARE(m_connection->redirectKeyCount(), count + 1);
    QCOMPARE(params.keyType, static_cast<int>(QEvent::KeyPress));
    QCOMPARE(params.keyCode, static_cast<int>(Qt::Key_Shift));
    QCOMPARE(params.text, QString(""));

    // no redirection should happen
    m_subject->setRedirectKeys(false);
    ret = m_subject->filterEvent(&request);
    QVERIFY(!ret);
    QCOMPARE(m_connection->redirectKeyCount(), count + 1);
    m_subject->setFocusWidget(0);
}

void Ut_MInputContext::waitAndProcessEvents(int waitTime)
{
    QTest::qWait(waitTime);
    while (app->hasPendingEvents()) {
        app->processEvents();
    }
}

void Ut_MInputContext::testInvalidScene()
{
    QGraphicsView view;

    // Regression test:
    // NB#177259 - MInputContext::setFocusWidget crash
    // commit f76915c6 - Prevent a crash in MInputContext if the QGraphicsView has no scene yet.
    m_subject->setFocusWidget(&view);

    m_subject->imInitiatedHide();
}

/* Test that when the input method hides,
 * the widget is unfocused.
 */
void Ut_MInputContext::testImInitiatedHideUnfocusesWidget()
{
    /* Due to using the windowing system, it is vunerable to race conditions;
     * the time it takes for the shown widget/window to become active and get focus may wary.
     * Could be made reliable by running the tests in a nested X server, or by having
     * a dummy windowing backend for Qt or otherwise mocking the Qt behavior.
     */
    QSKIP("Unreliable due to influence from windowing system", SkipSingle);

    WidgetStub widget(0);
    // If there is no toplevel window mapped, Qt will not actually focus the widget.
    m_subject->setFocusWidget(&widget);
    widget.show();
    widget.setFocus();
    waitAndProcessEvents(1000); // Focus changes is done async via eventloop
    QVERIFY(widget.hasFocus() == true); // Sanity check

    Q_EMIT m_connection->imInitiatedHide();
    waitAndProcessEvents(1000); // Focus changes is done async via eventloop

    QVERIFY(widget.hasFocus() == false);

    m_subject->setFocusWidget(0);
}

/* Test that when input method updates inputMethodArea,
 * the change is propagated to the extended input method interfaces.
 *
 * Note: As a side effect, this also tests that the extended input method interfaces
 * emit the appropriate signals when the input method area changes. */
void Ut_MInputContext::testImAreaChangePropagation()
{
    QRect newInputMethodArea(98, 99, 101, 102);
    QSignalSpy maliitInputMethodSpy(Maliit::InputMethod::instance(),
                                    SIGNAL(areaChanged(QRect)));

    Q_EMIT m_connection->updateInputMethodArea(newInputMethodArea);

    QCOMPARE(maliitInputMethodSpy.count(), 1);
    QCOMPARE(maliitInputMethodSpy.first().at(0).toRect(), newInputMethodArea);
}

/* Tests that when input method (through IM server) commits a string,
 * this results in an input method event carrying the commit string to the widget. */
void Ut_MInputContext::testImCommitStringInjection()
{
    QString committedString("Commit1");
    WidgetStub widget(0);
    m_subject->setFocusWidget(&widget);

    Q_EMIT m_connection->commitString(committedString);

    QInputMethodEvent event = widget.lastInputMethodEvent();
    QVERIFY(event.commitString() == committedString);

    m_subject->setFocusWidget(0);
}

/* Test that when the input method (through IM server) sets a selection,
 * this results in an input method event carrying the selection indices. */
void Ut_MInputContext::testImSelectionInjection()
{
    const int start = 2;
    const int length = 4;
    WidgetStub widget(0);
    m_subject->setFocusWidget(&widget);

    Q_EMIT m_connection->setSelection(start, length);

    QInputMethodEvent event = widget.lastInputMethodEvent();
    QList<QInputMethodEvent::Attribute> attributes = event.attributes();
    QCOMPARE(attributes.count(), 1);
    QInputMethodEvent::Attribute attribute(attributes.first());
    QVERIFY(attribute.type == QInputMethodEvent::Selection);
    QCOMPARE(attribute.start, start);
    QCOMPARE(attribute.length, length);

    m_subject->setFocusWidget(0);
}

/* Test that when the input method (through IM server) sets a selection
 * and then retrieves it, the correct value is returned.
 *
 * Note: Uses a real widget, and as a side effect also test whether this widget
 * responds to QWidget::inputMethodQuery(Qt::ImCurrentSelection) correctly. */
void Ut_MInputContext::testImGetSelection()
{
    const int start = 2;
    const int length = 8;
    QLineEdit widget(0);
    m_subject->setFocusWidget(&widget);
    widget.setText("xxSelectedxxx");
    widget.setSelection(start, length);
    QVERIFY(widget.selectedText() == "Selected"); // Sanity
    QString selection("init");
    bool validity = false;

    Q_EMIT m_connection->getSelection(selection, validity);

    QVERIFY(validity);
    QVERIFY(selection == "Selected");

    m_subject->setFocusWidget(0);
}

/* Test that MPreeditInjectionEvent is also accepted, for compatability purposes */
void Ut_MInputContext::testMPreeditInjectionEventCompatibility()
{
#ifdef HAVE_MEEGOTOUCH
    WidgetStub widget(0);
    gFocusedWidget = &widget;
    m_subject->setFocusWidget(&widget);
    m_subject->setGlobalCorrectionEnabled(true);

    QString preeditString("MPreeditInjection");

    MPreeditInjectionEvent injectionEvent(preeditString);
    bool accepted = QCoreApplication::sendEvent(m_subject, &injectionEvent);
    QCOMPARE(accepted, true);

    waitAndProcessEvents(500);

    QCOMPARE(m_connection->setPreeditCount(), 1);
    QCOMPARE(m_connection->lastPreedit(), preeditString);

    m_subject->setFocusWidget(0);
    gFocusedWidget = 0;
#else
    QSKIP("Not built against MeegoTouch, can't run test", SkipSingle);
#endif
}

void Ut_MInputContext::testPropertyNameNormalization_data()
{
    typedef QByteArray QBA;
    typedef QVariant QV;

    QTest::addColumn<QBA>("name");
    QTest::addColumn<QBA>("defaultName");
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QVariant>("expected");

    QTest::newRow("no normalization")
     << QBA("maliit-western-numeric-input-enforced")
     << QBA(Maliit::InputMethodQuery::westernNumericInputEnforced)
     << QV(true) << QV(true);

    QTest::newRow("Qt component normalization")
     << QBA("westernNumericInputEnforced")
     << QBA(Maliit::InputMethodQuery::westernNumericInputEnforced)
     << QV(true) << QV(true);

    QTest::newRow("Invalid property")
     << QBA("invalidPropertyName")
     << QBA("invalid-property-name")
     << QV(42) << QV();
}

void Ut_MInputContext::testPropertyNameNormalization()
{
    QFETCH(QByteArray, name);
    QFETCH(QByteArray, defaultName);
    QFETCH(QVariant, value);
    QFETCH(QVariant, expected);

    WidgetStub widget(0);
    gFocusedWidget = &widget;
    m_subject->setFocusWidget(&widget);

    widget.setProperty(name.data(), value);
    m_subject->update();
    QVariant extracted = m_subject->getStateInformation().value(QString(defaultName));

    QCOMPARE(extracted, expected);
}


QTEST_APPLESS_MAIN(Ut_MInputContext)

