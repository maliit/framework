#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <maliit/attributeextension.h>

#include <QMainWindow>
#include <QLabel>

class MainWindow
    : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    Q_SLOT void onEnableAllSubviewsToggled(bool toggle);

private:
    Maliit::AttributeExtension loadAllInputMethods;
};

#endif // MAINWINDOW_H
