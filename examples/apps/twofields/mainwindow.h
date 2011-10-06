#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>

#include "myextension.h"

class MainWindow
    : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private:
    QScopedPointer<MyExtension> loginExtension;
    QScopedPointer<MyExtension> passwordExtension;
};

#endif // MAINWINDOW_H
