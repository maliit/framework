#include <QKeyEvent>
#include <QLineEdit>
#include <QMessageBox>
#include <QString>

#include "actionkeyfilter.h"

ActionKeyFilter::ActionKeyFilter(QLineEdit *login, QLineEdit *password, QObject *parent)
:   QObject(parent),
    m_loginEdit(login),
    m_passwordEdit(password),
    m_enterLoginAccepts(true),
    m_enterPasswordAccepts(true)
{
    // EMPTY
}

bool ActionKeyFilter::enterLoginAccepts() const
{
    return m_enterLoginAccepts;
}

bool ActionKeyFilter::enterPasswordAccepts() const
{
    return m_enterPasswordAccepts;
}

void ActionKeyFilter::setEnterLoginAccepts(bool accepts)
{
    m_enterLoginAccepts = accepts;
}

void ActionKeyFilter::setEnterPasswordAccepts(bool accepts)
{
    m_enterPasswordAccepts = accepts;
}

bool ActionKeyFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (not event) {
        return false;
    }
    if (event->type() != QEvent::KeyPress) {
        return false;
    }

    QKeyEvent* keyEvent(static_cast<QKeyEvent*>(event));

    if (keyEvent->key() != Qt::Key_Return) {
        return false;
    }

    if ((obj == m_loginEdit) and m_enterLoginAccepts) {
        m_passwordEdit->setFocus(Qt::OtherFocusReason);
        return true;
    } else if ((obj == m_passwordEdit) and m_enterPasswordAccepts) {
        QString message;
        QMessageBox::Icon icon;

        if (m_loginEdit->text().isEmpty() || m_passwordEdit->text().isEmpty()) {
            message = "Please enter your credentials.";
            icon = QMessageBox::Warning;
        } else {
            message = "Login successfull!";
            icon = QMessageBox::Information;
        }

        QMessageBox messageBox;

        messageBox.setText(message);
        messageBox.addButton("OK", QMessageBox::AcceptRole);
        messageBox.setIcon(icon);
        messageBox.exec();
        return true;
    }
    return false;
}
