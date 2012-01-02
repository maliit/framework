#include "mainwindow.h"

#include <maliit/inputmethod.h>
#include <maliit/namespace.h>

#include <QApplication>
#include <QProcess>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVariant>
#include <QVBoxLayout>
#include <QRegExp>

namespace {

bool enableFullScreenMode()
{
    static const bool fullscreen(qApp->arguments().contains("-fullscreen"));
    return fullscreen;
}

}

MainWindow::MainWindow()
    : QMainWindow()
    , loadAllInputMethods()
{
    setWindowTitle("Maliit subview handling test application");

    QVBoxLayout *vbox = new QVBoxLayout;

    // Clicking the button will steal focus from the text edit, thus hiding
    // the virtual keyboard:
    QPushButton *hideVkb = new QPushButton("Hide virtual keyboard");
    vbox->addWidget(hideVkb);

    QLineEdit* entry = new QLineEdit;
    QCheckBox* enableAll = new QCheckBox("Enable all input methods");

    entry->setProperty(Maliit::InputMethodQuery::attributeExtensionId,
                       QVariant(loadAllInputMethods.id()));

    enableAll->setFocusProxy(entry);
    connect(enableAll, SIGNAL(toggled(bool)),
            this,    SLOT(onEnableAllSubviewsToggled(bool)));

    vbox->addWidget(entry);
    vbox->addWidget(enableAll);
    vbox->addStretch();

    QPushButton *closeApp = new QPushButton("Close application");
    vbox->addWidget(closeApp);
    connect(closeApp, SIGNAL(clicked()),
            this,     SLOT(close()));

    setCentralWidget(new QWidget);
    centralWidget()->setLayout(vbox);

    if (enableFullScreenMode()) {
        showFullScreen();
    } else {
        show();
    }
}

void MainWindow::onEnableAllSubviewsToggled(bool toggle)
{
    loadAllInputMethods.setAttribute("//inputMethod/loadAll", QVariant(toggle));
}
