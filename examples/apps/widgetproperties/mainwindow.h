#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>

class QTextEdit;

class MainWindow
    : public QMainWindow
{
    Q_OBJECT

private:
    QTextEdit *translucencyEdit;

public:
    MainWindow();
    Q_SLOT void onTranslucencyToggled(bool value);
};

#endif // MAINWINDOW_H
