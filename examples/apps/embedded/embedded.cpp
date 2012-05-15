#include "embedded.h"

#include <QtCore>
#include <QVBoxLayout>
#include <QApplication>
#include <QDesktopWidget>

#include <maliit/namespace.h>
#include <maliit/inputmethod.h>

MainWindow::MainWindow()
    : QMainWindow()
{
    initUI();
}

MainWindow::~MainWindow()
{}

void MainWindow::initUI()
{
    setWindowTitle("Maliit embedded demo");

    setCentralWidget(new QWidget);
    QVBoxLayout *vbox = new QVBoxLayout;

    QPushButton *closeApp = new QPushButton("Close application");
    vbox->addWidget(closeApp);
    connect(closeApp, SIGNAL(clicked()),
            this,     SLOT(close()));

    // Clicking the button will steal focus from the text edit, thus hiding
    // the virtual keyboard:
    QPushButton *hideVkb = new QPushButton("Hide virtual keyboard");
    vbox->addWidget(hideVkb);

    QTextEdit *textEdit = new QTextEdit;
    vbox->addWidget(textEdit);

    QWidget *imWidget = Maliit::InputMethod::instance()->widget();
    if (imWidget) {
        imWidget->setParent(centralWidget());
        vbox->addWidget(imWidget);
    } else {
        qCritical() << "Unable to embedded Maliit input method widget";
    }

    centralWidget()->setLayout(vbox);
    show();
}

