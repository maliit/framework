#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

namespace Maliit
{
  class AttributeExtension;
}

class MainWindow
    : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private:
    QScopedPointer<Maliit::AttributeExtension> loginExtension;
    QScopedPointer<Maliit::AttributeExtension> passwordExtension;
};

#endif // MAINWINDOW_H
