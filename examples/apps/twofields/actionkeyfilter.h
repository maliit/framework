#ifndef ACTION_KEY_FILTER_H
#define ACTION_KEY_FILTER_H

#include <QObject>

class QLineEdit;

class ActionKeyFilter : public QObject
{
    Q_OBJECT

public:
    ActionKeyFilter(QLineEdit *login, QLineEdit *password, QObject *parent = 0);
    bool enterLoginAccepts() const;
    bool enterPasswordAccepts() const;

public Q_SLOTS:
    void setEnterLoginAccepts(bool accepts);
    void setEnterPasswordAccepts(bool accepts);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    QLineEdit* m_loginEdit;
    QLineEdit* m_passwordEdit;
    bool m_enterLoginAccepts;
    bool m_enterPasswordAccepts;
};

#endif // ACTION_KEY_FILTER_H
