#include "ut_duiinputcontextdbusconnection.h"

#include <DuiApplication>
#include <QDebug>
#include <QDBusArgument>

#include "duiinputcontextadaptor_stub.h"
#include "duipreeditinjectionevent.h"


namespace
{
    const char *const DBusServiceName = "org.maemo.duiinputmethodserver1";
    const char *const DBusPath = "/org/maemo/duiinputmethodserver1";
    const char *const DBusInterface = "org.maemo.duiinputmethodserver1";

    // note: avoid path specified in DuiInputContext to avoid problems if it is instantiated
    const char *const DBusCallbackPath = "/org/maemo/duiinputcontextTEST";

    int DBusWaitTime = 500;

    const QString TestingInSandboxEnvVariable("TESTING_IN_SANDBOX");
}


//////////////////////////////////////
// Target for input context connection

TargetStub::TargetStub(DuiInputContextConnection *icConnection, QObject *parent)
    : DuiInputMethodBase(icConnection, parent)
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

void TargetStub::redirectKey(int keyType, int keyCode, const QString &text)
{
    Q_UNUSED(keyType);
    Q_UNUSED(keyCode);
    Q_UNUSED(text);
    m_redirectKeyCallCount++;
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
    m_redirectKeyCallCount = 0;
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

int TargetStub::redirectKeyCallCount()
{
    return m_redirectKeyCallCount;
}

//////////////
// actual test

void Ut_DuiInputContextDBusConnection::initTestCase()
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

    static char *argv[1] = { (char *) "ut_duiinputcontextdbusconnection" };
    static int argc = 1;

    DuiApplication::setLoadDuiInputContext(false);
    app = new DuiApplication(argc, argv);

    m_subject = new DuiInputContextDBusConnection;
    m_inputMethod = new TargetStub(m_subject);
    m_inputContext = new DuiInputContext(0);
    m_clientInterface = 0;

    if (!m_subject->isValid()) {
        QSKIP("DuiInputContextDBusConnection object not valid. Possibly other program using it running.",
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
    new DuiInputContextAdaptor(m_inputContext);
    connection.registerObject(contextObjectName, m_inputContext);

    m_clientInterface->call(QDBus::NoBlock, "setContextObject", contextObjectName);
    m_clientInterface->call(QDBus::NoBlock, "activateContext");

    // make sure dbus calls are handled before continuing to call things on m_subject
    handleMessages();
}


void Ut_DuiInputContextDBusConnection::cleanupTestCase()
{
    delete m_inputMethod;
    delete m_subject;
    delete m_clientInterface;
    delete app;
    delete m_inputContext;
}


void Ut_DuiInputContextDBusConnection::init()
{
    m_inputMethod->resetCallCounts();
    gDuiInputContextAdaptorStub->stubReset();
}


void Ut_DuiInputContextDBusConnection::cleanup()
{
}



// test methods:

// tests that calls get through dbus
void Ut_DuiInputContextDBusConnection::testNoReplyDBusCalls()
{
    QString preedit("preedit string");
    m_subject->sendPreeditString(preedit, PreeditDefault);
    handleMessages();
    QCOMPARE(gDuiInputContextAdaptorStub->stubCallCount("updatePreedit"), 1);
    QCOMPARE(gDuiInputContextAdaptorStub->stubLastCallTo("updatePreedit").parameter<QString>(0), preedit);
    QCOMPARE(gDuiInputContextAdaptorStub->stubLastCallTo("updatePreedit").parameter<int>(1), (int)PreeditDefault);

    m_subject->sendPreeditString(preedit, PreeditNoCandidates);
    handleMessages();
    QCOMPARE(gDuiInputContextAdaptorStub->stubCallCount("updatePreedit"), 2);
    QCOMPARE(gDuiInputContextAdaptorStub->stubLastCallTo("updatePreedit").parameter<QString>(0), preedit);
    QCOMPARE(gDuiInputContextAdaptorStub->stubLastCallTo("updatePreedit").parameter<int>(1), (int)PreeditNoCandidates);

    QString commit("commit string");
    m_subject->sendCommitString(commit);
    handleMessages();
    QCOMPARE(gDuiInputContextAdaptorStub->stubCallCount("commitString"), 1);
    QCOMPARE(gDuiInputContextAdaptorStub->stubLastCallTo("commitString").parameter<QString>(0), commit);

    m_subject->sendKeyEvent(QKeyEvent(QEvent::KeyPress, 0, 0, QString(), false, 0));
    handleMessages();
    QCOMPARE(gDuiInputContextAdaptorStub->stubCallCount("keyEvent"), 1);

    m_subject->notifyImInitiatedHiding();
    handleMessages();
    QCOMPARE(gDuiInputContextAdaptorStub->stubCallCount("imInitiatedHide"), 1);

    QRegion region(0, 0, 10, 10);
    m_subject->updateInputMethodArea(region);
    handleMessages();
    QCOMPARE(gDuiInputContextAdaptorStub->stubCallCount("updateInputMethodArea"), 1);
    QList<QVariant> rectList = gDuiInputContextAdaptorStub->stubLastCallTo("updateInputMethodArea").parameter< QList<QVariant> >(0);
    QCOMPARE(rectList.length(), 1);
    QCOMPARE(region.rects().at(0), qdbus_cast<QRect>(rectList.at(0)));

    m_subject->setGlobalCorrectionEnabled(true);
    handleMessages();
    QCOMPARE(gDuiInputContextAdaptorStub->stubCallCount("setGlobalCorrectionEnabled"), 1);
    QVERIFY(gDuiInputContextAdaptorStub->stubLastCallTo("setGlobalCorrectionEnabled").parameter<bool>(0));

    m_subject->setRedirectKeys(true);
    handleMessages();
    QCOMPARE(gDuiInputContextAdaptorStub->stubCallCount("setRedirectKeys"), 1);
}


void Ut_DuiInputContextDBusConnection::testShowOnFocus()
{
    m_clientInterface->call(QDBus::NoBlock, "showInputMethod");

    handleMessages();

    QCOMPARE(m_inputMethod->showCallCount(), 1);
}


void Ut_DuiInputContextDBusConnection::testHideOnLostFocus()
{
    m_clientInterface->call(QDBus::NoBlock, "hideInputMethod");

    handleMessages();

    QCOMPARE(m_inputMethod->hideCallCount(), 1);
}


void Ut_DuiInputContextDBusConnection::testMouseClickedOnPreedit()
{
    QPoint pos(1, 1);
    QRect rect;
    m_clientInterface->call(QDBus::NoBlock, "mouseClickedOnPreedit", pos, rect);

    handleMessages();

    QCOMPARE(m_inputMethod->mouseClickedOnPreeditCallCount(), 1);
}


void Ut_DuiInputContextDBusConnection::testSetPreedit()
{
    QString preedit("preedit string");
    m_clientInterface->call(QDBus::NoBlock, "setPreedit", preedit);

    handleMessages();

    QCOMPARE(m_inputMethod->setPreeditCallCount(), 1);
}


void Ut_DuiInputContextDBusConnection::testReset()
{
    m_clientInterface->call(QDBus::NoBlock, "reset");

    handleMessages();

    QCOMPARE(m_inputMethod->resetCallCount(), 1);
}

void Ut_DuiInputContextDBusConnection::testAppOrientationChanged()
{
    m_clientInterface->call(QDBus::NoBlock, "appOrientationChanged", 90);

    handleMessages();

    QCOMPARE(m_inputMethod->appOrientationChangedCallCount(), 1);
}

void Ut_DuiInputContextDBusConnection::testRedirectKey()
{
    m_clientInterface->call(QDBus::NoBlock, "redirectKey", 0, 0, "");

    handleMessages();

    QCOMPARE(m_inputMethod->redirectKeyCallCount(), 1);
}

void Ut_DuiInputContextDBusConnection::handleMessages()
{
    QTest::qWait(DBusWaitTime);
    while (app->hasPendingEvents()) {
        app->processEvents();
    }
}



QTEST_APPLESS_MAIN(Ut_DuiInputContextDBusConnection)
