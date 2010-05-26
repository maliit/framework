#include "ut_minputcontextdbusconnection.h"

#include <MApplication>
#include <QDebug>
#include <QDBusArgument>

#include "minputcontextadaptor_stub.h"
#include "mpreeditinjectionevent.h"


namespace
{
    const char *const DBusServiceName = "org.maemo.duiinputmethodserver1";
    const char *const DBusPath = "/org/maemo/duiinputmethodserver1";
    const char *const DBusInterface = "org.maemo.duiinputmethodserver1";

    // note: avoid path specified in MInputContext to avoid problems if it is instantiated
    const char *const DBusCallbackPath = "/org/maemo/duiinputcontextTEST";

    int DBusWaitTime = 500;

    const QString TestingInSandboxEnvVariable("TESTING_IN_SANDBOX");
}


//////////////////////////////////////
// Target for input context connection

TargetStub::TargetStub(MInputContextConnection *icConnection, QObject *parent)
    : MInputMethodBase(icConnection, parent)
{
    resetCallCounts();
}

TargetStub::~TargetStub()
{
}

void TargetStub::show()
{
    m_showCallCount++;
}

void TargetStub::hide()
{
    m_hideCallCount++;
}

void TargetStub::setPreedit(const QString &preeditString)
{
    Q_UNUSED(preeditString)

    m_setPreeditCallCount++;
}

void TargetStub::reset()
{
    m_resetCallCount++;
}

void TargetStub::mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect)
{
    Q_UNUSED(pos);
    Q_UNUSED(preeditRect);

    m_mouseClickedOnPreeditCallCount++;
}

void TargetStub::update()
{
    m_updateCallCount++;
}

void TargetStub::visualizationPriorityChanged(bool enabled)
{
    Q_UNUSED(enabled);
    m_visualizationPriorityChangedCallCount++;
}

void TargetStub::appOrientationChanged(int angle)
{
    Q_UNUSED(angle);
    m_appOrientationChangedCallCount++;
}

void TargetStub::processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                 Qt::KeyboardModifiers modifiers, const QString &text,
                                 bool autoRepeat, int count, quint32 nativeScanCode,
                                 quint32 nativeModifiers)
{
    Q_UNUSED(keyType);
    Q_UNUSED(keyCode);
    Q_UNUSED(modifiers);
    Q_UNUSED(text);
    Q_UNUSED(autoRepeat);
    Q_UNUSED(count);
    Q_UNUSED(nativeScanCode);
    Q_UNUSED(nativeModifiers);

    m_processKeyEventCallCount++;
}

void TargetStub::resetCallCounts()
{
    m_showCallCount = 0;
    m_hideCallCount = 0;
    m_setPreeditCallCount = 0;
    m_resetCallCount = 0;
    m_mouseClickedOnPreeditCallCount = 0;
    m_updateCallCount = 0;
    m_visualizationPriorityChangedCallCount = 0;
    m_appOrientationChangedCallCount = 0;
    m_processKeyEventCallCount = 0;
}

int TargetStub::showCallCount()
{
    return m_showCallCount;
}

int TargetStub::hideCallCount()
{
    return m_hideCallCount;
}

int TargetStub::setPreeditCallCount()
{
    return m_setPreeditCallCount;
}

int TargetStub::resetCallCount()
{
    return m_resetCallCount;
}

int TargetStub::mouseClickedOnPreeditCallCount()
{
    return m_mouseClickedOnPreeditCallCount;
}

int TargetStub::updateCallCount()
{
    return m_updateCallCount;
}

int TargetStub::visualizationPriorityChangedCallCount()
{
    return m_visualizationPriorityChangedCallCount;
}

int TargetStub::appOrientationChangedCallCount()
{
    return m_appOrientationChangedCallCount;
}

int TargetStub::processKeyEventCallCount()
{
    return m_processKeyEventCallCount;
}

//////////////
// actual test

void Ut_MInputContextDBusConnection::initTestCase()
{
    // This is a hack to prevent Qt from loading the plugin from
    // /usr/lib/qt4/plugins/inputmethods/ when we are testing in a
    // sandbox.
    bool testingInSandbox = false;
    const QStringList env(QProcess::systemEnvironment());
    int index = env.indexOf(QRegExp('^' + TestingInSandboxEnvVariable + "=.*", Qt::CaseInsensitive));
    if (index != -1) {
        QString statusCandidate = env.at(index);
        statusCandidate = statusCandidate.remove(
                              QRegExp('^' + TestingInSandboxEnvVariable + '=', Qt::CaseInsensitive));
        bool statusOk = false;
        int status = statusCandidate.toInt(&statusOk);
        if (statusOk && (status == 0 || status == 1)) {
            testingInSandbox = (status == 1);
        } else {
            qDebug() << "Invalid " << TestingInSandboxEnvVariable << " environment variable.\n";
            QFAIL("Attempt to execute test incorrectly.");
        }
    }
    if (testingInSandbox)
        QCoreApplication::setLibraryPaths(QStringList("/tmp"));

    static char *argv[1] = { (char *) "ut_minputcontextdbusconnection" };
    static int argc = 1;

    MApplication::setLoadMInputContext(false);
    app = new MApplication(argc, argv);

    m_subject = new MInputContextDBusConnection;
    m_inputMethod = new TargetStub(m_subject);
    m_inputContext = new MInputContext(0);
    m_clientInterface = 0;

    if (!m_subject->isValid()) {
        QSKIP("MInputContextDBusConnection object not valid. Possibly other program using it running.",
              SkipAll);
    }

    m_subject->addTarget(m_inputMethod);

    // init dbus client
    if (!QDBusConnection::sessionBus().isConnected()) {
        QFAIL("Cannot connect to the DBus session bus");
    }

    QDBusConnection connection = QDBusConnection::sessionBus();
    m_clientInterface = new QDBusInterface(DBusServiceName, DBusPath, DBusInterface, connection, 0);

    if (!m_clientInterface->isValid()) {
        QFAIL(qPrintable(QDBusConnection::sessionBus().lastError().message()));
    }

    // connect methods we offer via DBus
    QString contextObjectName(DBusCallbackPath);
    new MInputContextAdaptor(m_inputContext);
    connection.registerObject(contextObjectName, m_inputContext);

    m_clientInterface->call(QDBus::NoBlock, "setContextObject", contextObjectName);
    m_clientInterface->call(QDBus::NoBlock, "activateContext");

    // make sure dbus calls are handled before continuing to call things on m_subject
    handleMessages();
}


void Ut_MInputContextDBusConnection::cleanupTestCase()
{
    delete m_inputMethod;
    delete m_subject;
    delete m_clientInterface;
    delete app;
    delete m_inputContext;
}


void Ut_MInputContextDBusConnection::init()
{
    m_inputMethod->resetCallCounts();
    gMInputContextAdaptorStub->stubReset();
}


void Ut_MInputContextDBusConnection::cleanup()
{
}



// test methods:

// tests that calls get through dbus
void Ut_MInputContextDBusConnection::testNoReplyDBusCalls()
{
    QString preedit("preedit string");
    m_subject->sendPreeditString(preedit, PreeditDefault);
    handleMessages();
    QCOMPARE(gMInputContextAdaptorStub->stubCallCount("updatePreedit"), 1);
    QCOMPARE(gMInputContextAdaptorStub->stubLastCallTo("updatePreedit").parameter<QString>(0, ""), preedit);
    QCOMPARE(gMInputContextAdaptorStub->stubLastCallTo("updatePreedit").parameter<int>(1, -1), (int)PreeditDefault);

    m_subject->sendPreeditString(preedit, PreeditNoCandidates);
    handleMessages();
    QCOMPARE(gMInputContextAdaptorStub->stubCallCount("updatePreedit"), 2);
    QCOMPARE(gMInputContextAdaptorStub->stubLastCallTo("updatePreedit").parameter<QString>(0, ""), preedit);
    QCOMPARE(gMInputContextAdaptorStub->stubLastCallTo("updatePreedit").parameter<int>(1, -1), (int)PreeditNoCandidates);

    QString commit("commit string");
    m_subject->sendCommitString(commit);
    handleMessages();
    QCOMPARE(gMInputContextAdaptorStub->stubCallCount("commitString"), 1);
    QCOMPARE(gMInputContextAdaptorStub->stubLastCallTo("commitString").parameter<QString>(0, ""), commit);

    m_subject->sendKeyEvent(QKeyEvent(QEvent::KeyPress, 0, 0, QString(), false, 0));
    handleMessages();
    QCOMPARE(gMInputContextAdaptorStub->stubCallCount("keyEvent"), 1);

    m_subject->notifyImInitiatedHiding();
    handleMessages();
    QCOMPARE(gMInputContextAdaptorStub->stubCallCount("imInitiatedHide"), 1);

    QRegion region(0, 0, 10, 10);
    m_subject->updateInputMethodArea(region);
    handleMessages();
    QCOMPARE(gMInputContextAdaptorStub->stubCallCount("updateInputMethodArea"), 1);
    QList<QVariant> rectList = gMInputContextAdaptorStub->stubLastCallTo("updateInputMethodArea").parameter< QList<QVariant> >(0, QList<QVariant>());
    QCOMPARE(rectList.length(), 1);
    QCOMPARE(region.rects().at(0), qdbus_cast<QRect>(rectList.at(0)));

    m_subject->setGlobalCorrectionEnabled(true);
    handleMessages();
    QCOMPARE(gMInputContextAdaptorStub->stubCallCount("setGlobalCorrectionEnabled"), 1);
    QVERIFY(gMInputContextAdaptorStub->stubLastCallTo("setGlobalCorrectionEnabled").parameter<bool>(0, false));

    m_subject->setRedirectKeys(true);
    handleMessages();
    QCOMPARE(gMInputContextAdaptorStub->stubCallCount("setRedirectKeys"), 1);
}


void Ut_MInputContextDBusConnection::testShowOnFocus()
{
    m_clientInterface->call(QDBus::NoBlock, "showInputMethod");

    handleMessages();

    QCOMPARE(m_inputMethod->showCallCount(), 1);
}


void Ut_MInputContextDBusConnection::testHideOnLostFocus()
{
    m_clientInterface->call(QDBus::NoBlock, "hideInputMethod");

    handleMessages();

    QCOMPARE(m_inputMethod->hideCallCount(), 1);
}


void Ut_MInputContextDBusConnection::testMouseClickedOnPreedit()
{
    QPoint pos(1, 1);
    QRect rect;
    m_clientInterface->call(QDBus::NoBlock, "mouseClickedOnPreedit", pos, rect);

    handleMessages();

    QCOMPARE(m_inputMethod->mouseClickedOnPreeditCallCount(), 1);
}


void Ut_MInputContextDBusConnection::testSetPreedit()
{
    QString preedit("preedit string");
    m_clientInterface->call(QDBus::NoBlock, "setPreedit", preedit);

    handleMessages();

    QCOMPARE(m_inputMethod->setPreeditCallCount(), 1);
}


void Ut_MInputContextDBusConnection::testReset()
{
    m_clientInterface->call(QDBus::NoBlock, "reset");

    handleMessages();

    QCOMPARE(m_inputMethod->resetCallCount(), 1);
}

void Ut_MInputContextDBusConnection::testAppOrientationChanged()
{
    m_clientInterface->call(QDBus::NoBlock, "appOrientationChanged", 90);

    handleMessages();

    QCOMPARE(m_inputMethod->appOrientationChangedCallCount(), 1);
}

void Ut_MInputContextDBusConnection::testProcessKeyEvent()
{
    m_clientInterface->call(QDBus::NoBlock, "processKeyEvent", 0, 0, 0, "", false, 0, 0, 0);

    handleMessages();

    QCOMPARE(m_inputMethod->processKeyEventCallCount(), 1);
}

void Ut_MInputContextDBusConnection::handleMessages()
{
    QTest::qWait(DBusWaitTime);
    while (app->hasPendingEvents()) {
        app->processEvents();
    }
}



QTEST_APPLESS_MAIN(Ut_MInputContextDBusConnection)
