#include <QX11Info>
#include <cstdlib>

#include <maliit/attributeextension.h>
#include <maliit/inputmethod.h>
#include <maliit/namespace.h>

#include "mainwindow.h"

namespace {
    bool enableFullScreenMode()
    {
        static const bool fullscreen(qApp->arguments().contains("-fullscreen"));
        return fullscreen;
    }

    class ActionKeyFilter : public QObject
    {
    public:
        explicit ActionKeyFilter(QLineEdit *login, QLineEdit *password, QObject *parent)
        : QObject(parent),
          loginEdit(login),
          passwordEdit(password)
        {}
    protected:
        bool eventFilter(QObject *obj, QEvent *event)
        {
            if (!event)
            {
                return false;
            }
            if (event->type() != QEvent::KeyPress)
            {
                return false;
            }

            QKeyEvent* keyEvent(static_cast<QKeyEvent*>(event));

            if (keyEvent->key() != Qt::Key_Return)
            {
                return false;
            }

            if (obj == loginEdit)
            {
                passwordEdit->setFocus(Qt::OtherFocusReason);
            }
            else if (obj == passwordEdit)
            {
                QString message;
                QMessageBox::Icon icon;

                if (loginEdit->text().isEmpty() || passwordEdit->text().isEmpty())
                {
                    message = "Please enter your credentials.";
                    icon = QMessageBox::Warning;
                }
                else
                {
                    message = "Login successfull!";
                    icon = QMessageBox::Information;
                }

                QMessageBox messageBox;

                messageBox.setText(message);
                messageBox.addButton("OK", QMessageBox::AcceptRole);
                messageBox.setIcon(icon);
                messageBox.exec();
            }
            return false;
        }
    private:
        QLineEdit* loginEdit;
        QLineEdit* passwordEdit;
    };
}

MainWindow::MainWindow()
    : QMainWindow()
    , loginExtension(new Maliit::AttributeExtension)
    , passwordExtension(new Maliit::AttributeExtension)
{
    setWindowTitle("Maliit key override test application");

    QVBoxLayout *vbox = new QVBoxLayout();

    // Clicking the button will steal focus from the text edit, thus hiding
    // the virtual keyboard:
    QPushButton *hideVkb = new QPushButton("Hide virtual keyboard");
    vbox->addWidget(hideVkb);

    QLabel* loginLabel(new QLabel("Login:"));
    QLineEdit* loginEdit(new QLineEdit());
    QLabel* passwordLabel(new QLabel("Password:"));
    QLineEdit* passwordEdit(new QLineEdit());
    QLabel* commentLabel(new QLabel("Comment (not required):"));
    QLineEdit* commentEdit(new QLineEdit());
    ActionKeyFilter* filter(new ActionKeyFilter(loginEdit, passwordEdit, this));

    loginEdit->installEventFilter(filter);
    loginEdit->setProperty(Maliit::InputMethodQuery::attributeExtensionId,
                           QVariant(loginExtension->id()));
    loginExtension->setAttribute("/keys/actionKey/label", "Next");
    passwordEdit->installEventFilter(filter);
    passwordEdit->setProperty(Maliit::InputMethodQuery::attributeExtensionId,
                              QVariant(passwordExtension->id()));
    passwordExtension->setAttribute("/keys/actionKey/label", "Login");
    passwordEdit->setEchoMode(QLineEdit::Password);

    vbox->addWidget(loginLabel);
    vbox->addWidget(loginEdit);
    vbox->addWidget(passwordLabel);
    vbox->addWidget(passwordEdit);
    vbox->addWidget(commentLabel);
    vbox->addWidget(commentEdit);

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

MainWindow::~MainWindow()
{}
