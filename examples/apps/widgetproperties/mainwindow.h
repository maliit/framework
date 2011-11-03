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
    QTextEdit *entryWithProperties;

public:
    MainWindow();
    Q_SLOT void onTranslucencyToggled(bool value);
    Q_SLOT void onPreferNumbersToggled(bool value);
};

#endif // MAINWINDOW_H
