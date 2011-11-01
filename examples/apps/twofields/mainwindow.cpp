#include <cstdlib>

#include <QApplication>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVariant>
#include <QVBoxLayout>
#include <QX11Info>

#include <maliit/inputmethod.h>
#include <maliit/namespace.h>

#include "actionkeyfilter.h"
#include "mainwindow.h"

namespace {
    bool enableFullScreenMode()
    {
        static const bool fullscreen(qApp->arguments().contains("-fullscreen"));
        return fullscreen;
    }
}

MainWindow::MainWindow()
    : QMainWindow(),
      loginExtension(new MyExtension("Next")),
      passwordExtension(new MyExtension("Login"))
{
    setWindowTitle("Maliit key override test application");

    QVBoxLayout *vbox = new QVBoxLayout();

    // Clicking the button will steal focus from the text edit, thus hiding
    // the virtual keyboard:
    QPushButton *hideVkb = new QPushButton("Hide virtual keyboard");
    vbox->addWidget(hideVkb);

    QLabel* loginLabel(new QLabel("Login:"));
    QLineEdit* loginEdit(new QLineEdit());
    loginEdit->setInputMethodHints(Qt::ImhPreferNumbers);
    QCheckBox* loginCheckBox(new QCheckBox("Enter accepts login"));
    QLabel* passwordLabel(new QLabel("Password:"));
    QLineEdit* passwordEdit(new QLineEdit());
    QCheckBox* passwordCheckBox(new QCheckBox("Enter accepts password"));
    QLabel* commentLabel(new QLabel("Comment (not required):"));
    QLineEdit* commentEdit(new QLineEdit());
    ActionKeyFilter* filter(new ActionKeyFilter(loginEdit, passwordEdit, this));

    loginEdit->installEventFilter(filter);
    loginEdit->setProperty(Maliit::InputMethodQuery::attributeExtensionId,
                           QVariant(loginExtension->id()));
    loginCheckBox->setFocusProxy(loginEdit);
    connect(loginCheckBox, SIGNAL(toggled(bool)),
            loginExtension.data(), SLOT(overrideLabel(bool)));
    loginCheckBox->setChecked(filter->enterLoginAccepts());
    connect(loginCheckBox, SIGNAL(toggled(bool)),
            filter, SLOT(setEnterLoginAccepts(bool)));

    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->installEventFilter(filter);
    passwordEdit->setProperty(Maliit::InputMethodQuery::attributeExtensionId,
                              QVariant(passwordExtension->id()));
    passwordCheckBox->setFocusProxy(passwordEdit);
    connect(passwordCheckBox, SIGNAL(toggled(bool)),
            passwordExtension.data(), SLOT(overrideLabel(bool)));
    passwordCheckBox->setChecked(filter->enterPasswordAccepts());
    connect(passwordCheckBox, SIGNAL(toggled(bool)),
            filter, SLOT(setEnterPasswordAccepts(bool)));

    vbox->addWidget(loginLabel);
    vbox->addWidget(loginEdit);
    vbox->addWidget(loginCheckBox);
    vbox->addWidget(passwordLabel);
    vbox->addWidget(passwordEdit);
    vbox->addWidget(passwordCheckBox);
    vbox->addWidget(commentLabel);
    vbox->addWidget(commentEdit);
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
