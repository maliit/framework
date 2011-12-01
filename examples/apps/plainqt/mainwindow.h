#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QPushButton>
#include <QTextEdit>

class MainWindow
        : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow();

private Q_SLOTS:
    void onStartServerClicked();
    void onRotateKeyboardClicked();
    void onServerError();
    void onServerStateChanged();

private:
    void initUI();

    QProcess *serverProcess;
    int orientationIndex;
    QPushButton *startServerButton;
    QPushButton *rotateKeyboardButton;
    QTextEdit *textEdit;
};

#endif // MAINWINDOW_H
