#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtGui>

class MainWindow
        : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow();

private slots:
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
