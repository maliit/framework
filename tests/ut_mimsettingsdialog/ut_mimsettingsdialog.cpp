#include "ut_mimsettingsdialog.h"

#include "mgconfitem_stub.h"
#include "minputcontextconnection_stub.h"
#include "fakegconf.h"

#include <QProcess>
#include <QGraphicsScene>

#include <mimsettingsdialog.h>
#include <mimpluginmanager.h>
#include <mimpluginmanager_p.h>
#include <mplainwindow.h>
#include <MDialog>
#include <MPopupList>
#include <MSceneManager>
#include <MApplication>
#include <QDir>
#include <QStandardItemModel>
#include <QSignalSpy>
#include <QVariant>
#include <QDebug>

namespace {
    const QString GlobalTestPluginPath("/usr/lib/meego-im-framework-tests/plugins");
    const QString TestPluginPathEnvVariable("TESTPLUGIN_PATH");

    const QString ConfigRoot          = "/meegotouch/inputmethods/";
    const QString MImPluginPaths    = ConfigRoot + "paths";
    const QString MImPluginDisabled = ConfigRoot + "disabledpluginfiles";

    const QString PluginRoot          = "/meegotouch/inputmethods/plugins/";

    const QString PluginName  = "DummyImPlugin";
    const QString PluginName2 = "DummyImPlugin2";
    const QString PluginName3 = "DummyImPlugin3";
}

// Stubbing..................................................................

int MSceneManager::execDialog(MDialog *dialog)
{
    // avoid MSceneManager create private even loop
    appearSceneWindowNow(dialog);
    return 0;
}

void Ut_MIMSettingsDialog::initTestCase()
{
    static char *argv[1] = { (char *) "ut_mimsettingsdialog" };
    static int argc = 1;

    // Avoid waiting if im server is not responding
    MApplication::setLoadMInputContext(false);

    app = new MApplication(argc, argv);

    // Use either global test plugin directory or TESTPLUGIN_PATH, if it is
    // set (to local sandbox's plugin directory by makefile, at least).
    pluginPath = GlobalTestPluginPath;

    QString pathCandidate = QProcessEnvironment::systemEnvironment().value(TestPluginPathEnvVariable);
    QVERIFY2(QDir(pluginPath).exists(), "Test plugin directory does not exist.");

    new MPlainWindow;
}

void Ut_MIMSettingsDialog::cleanupTestCase()
{
    delete MPlainWindow::instance();
    delete app;
}

void Ut_MIMSettingsDialog::init()
{
    MGConfItem pathConf(MImPluginPaths);
    pathConf.set(pluginPath);

    MGConfItem handlerItem(PluginRoot + "onscreen");
    handlerItem.set(PluginName);

    pluginMgr = new MIMPluginManager();
    subject = new MIMSettingsDialog(pluginMgr->d_ptr,
                     "", M::NoStandardButton);
    pluginMgr->d_ptr->settingsDialog = subject;
    MPlainWindow::instance()->sceneManager()->appearSceneWindowNow(subject);
}

void Ut_MIMSettingsDialog::cleanup()
{
    MPlainWindow::instance()->sceneManager()->disappearSceneWindowNow(subject);
    delete subject;
    subject = 0;
    pluginMgr->d_ptr->settingsDialog = 0;
    delete pluginMgr;
    pluginMgr = 0;
}

void Ut_MIMSettingsDialog::testShowAvailableSubViewList()
{
    QString activePlugin = pluginMgr->activePluginsName(OnScreen);
    QVERIFY(activePlugin == PluginName);

    QMap<QString, QString> availableSubviews = pluginMgr->availableSubViews(activePlugin, OnScreen);
    QCOMPARE(availableSubviews.count(), 2);

    pluginMgr->setActiveSubView(QString("dummyimsv2"), OnScreen);
    QCOMPARE(pluginMgr->activeSubView(OnScreen), QString("dummyimsv2"));

    subject->showAvailableSubViewList();

    QVERIFY(subject->availableSubViewList);
    QVERIFY(subject->availableSubViewList->isVisible());

    QStandardItemModel *model = static_cast<QStandardItemModel*> (subject->availableSubViewList->itemModel());
    QVERIFY(model->rowCount() > 2);

    QList<QStandardItem *> items = model->findItems(QString("dummyimsv2"));
    QCOMPARE(items.count(), 1);
    QCOMPARE(items[0]->index(), subject->availableSubViewList->currentIndex());
}

void Ut_MIMSettingsDialog::testSetActiveView()
{
    subject->showAvailableSubViewList();
    QVERIFY(subject->availableSubViewList);
    QVERIFY(subject->availableSubViewList->isVisible());

    QStandardItemModel *model = static_cast<QStandardItemModel*> (subject->availableSubViewList->itemModel());
    QVERIFY(model->rowCount() > 2);

    QList<QStandardItem *> items = model->findItems(QString("dummyimsv2"));
    QCOMPARE(items.count(), 1);
    subject->availableSubViewList->click(items[0]->index());

    QCOMPARE(pluginMgr->activeSubView(OnScreen), QString("dummyimsv2"));
}

void Ut_MIMSettingsDialog::testRetranslateUi()
{
    QSignalSpy spy(subject, SIGNAL(languageChanged()));
    subject->retranslateUi();
    QCOMPARE(spy.count(), 1);
}

void Ut_MIMSettingsDialog::testHandleDialogDisappeared()
{
    subject->showAvailableSubViewList();
    QVERIFY(subject->availableSubViewList);
    QVERIFY(subject->availableSubViewList->isVisible());

    QSignalSpy spy(subject->availableSubViewList, SIGNAL(rejected()));
    MPlainWindow::instance()->sceneManager()->disappearSceneWindowNow(subject);
    QCOMPARE(spy.count(), 1);
}

QTEST_APPLESS_MAIN(Ut_MIMSettingsDialog)

