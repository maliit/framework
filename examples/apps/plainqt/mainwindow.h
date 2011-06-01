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
    void onServerError();
    void onServerStateChanged();

private:
    void initUI();
    QProcess *serverProcess;
    QPushButton *startServerButton;
    QTextEdit *textEdit;
};

#endif // MAINWINDOW_H
