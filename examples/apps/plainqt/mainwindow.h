#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

class MainWindow
    : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    virtual ~MainWindow();

private:
    Q_SLOT void onStartServerClicked();
    Q_SLOT void onRotateKeyboardClicked();
    Q_SLOT void onServerError();
    Q_SLOT void onServerStateChanged();

private:
    void initUI();

    QProcess *m_server_process;
    int m_orientation_index;
    QPushButton *m_start_server;
    QPushButton *m_rotate_keyboard;
    QTextEdit *m_entry;
};

#endif // MAINWINDOW_H
