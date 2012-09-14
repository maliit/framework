#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Qt/qglobal.h>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtGui/QFocusEvent>
#include <QtGui/QInputMethod>
#include <QtCore/QProcess>
#else
#include <QtGui>
#endif

class MainWindow
    : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    virtual ~MainWindow();

    bool eventFilter(QObject *watched,
                     QEvent *event);

private:
    Q_SLOT void onStartServerClicked();
    Q_SLOT void onRotateKeyboardClicked();
    Q_SLOT void onServerStateChanged();

private:
    void initUI();
    void insertIntoGrid(const QString &description,
                        const Qt::InputMethodHints &hints,
                        const QString &tooltip = QString());

    QProcess *m_server_process;
    int m_orientation_index;
    int m_grid_row;
    QGridLayout *const m_grid;
    QPushButton *const m_start_server;
    QPushButton *const m_rotate_keyboard;
};

#endif // MAINWINDOW_H
