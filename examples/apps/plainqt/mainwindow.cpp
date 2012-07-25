#include "mainwindow.h"

#include <QtCore>
#include <QVBoxLayout>

#if defined(Q_WS_X11)
#include <QX11Info>
#endif

#include <maliit/namespace.h>
#include <maliit/inputmethod.h>

#include <cstdlib>

namespace {
    const char * const TextPrompt = "Double-click this text area to invoke virtual keyboard ...";
    const QString serverName("maliit-server"); //TODO: when maliit and example application is split out, look up in .pc file

    bool enableFullScreenMode()
    {
        return (qApp->arguments().contains("-fullscreen"));
    }
}

class MyTextEdit
    : public QTextEdit
{
private:
    bool wasFocused;

public:
    MyTextEdit()
        : QTextEdit(TextPrompt)
        , wasFocused(false)
    {}

protected:
    void focusInEvent(QFocusEvent *e)
    {
        toPlainText();
        // On first text edit, clear pre-set TextPrompt:
        if (not wasFocused && toPlainText() == QString(TextPrompt)) {
            wasFocused = true;
            setText("");
        }

        QTextEdit::focusInEvent(e);
    }
};

MainWindow::MainWindow()
    : QMainWindow()
    , serverProcess(new QProcess(this))
    , orientationIndex(0)
    , startServerButton(new QPushButton)
    , rotateKeyboardButton(new QPushButton)
    , textEdit(new QTextEdit)
{
    serverProcess->setProcessChannelMode(QProcess::ForwardedChannels);
    connect(serverProcess, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(onServerError()));
    connect(serverProcess, SIGNAL(stateChanged(QProcess::ProcessState)),
            this, SLOT(onServerStateChanged()));

    connect(startServerButton, SIGNAL(clicked()),
            this, SLOT(onStartServerClicked()));
    connect(rotateKeyboardButton, SIGNAL(clicked()),
            this, SLOT(onRotateKeyboardClicked()));

    initUI();
    onServerStateChanged();
}

void MainWindow::initUI()
{
    setWindowTitle("Maliit test application");

    QVBoxLayout *vbox = new QVBoxLayout();

    rotateKeyboardButton->setText("Rotate keyboard");
    vbox->addWidget(startServerButton);
    vbox->addWidget(rotateKeyboardButton);

    // Clicking the button will steal focus from the text edit, thus hiding
    // the virtual keyboard:
    QPushButton *hideVkb = new QPushButton("Hide virtual keyboard");
    vbox->addWidget(hideVkb);
    vbox->addWidget(textEdit);

    // But don't want other buttons to steal focus:
    startServerButton->setFocusProxy(textEdit);
    rotateKeyboardButton->setFocusProxy(textEdit);

    QPushButton *closeApp = new QPushButton("Close application");
    vbox->addWidget(closeApp);
    connect(closeApp, SIGNAL(clicked()),
            this,     SLOT(close()));

    setCentralWidget(new QWidget);
    centralWidget()->setLayout(vbox);

    if (enableFullScreenMode()) {
        showFullScreen();
    } else {
        show();
    }
}

MainWindow::~MainWindow()
{
    serverProcess->terminate();
}

void MainWindow::onStartServerClicked()
{

    QStringList arguments;
    arguments << "-bypass-wm-hint";

    // Self-compositing is currently only supported in fullscreen mode:
    if (enableFullScreenMode()) {
        arguments << "-use-self-composition";
    }

    if (serverProcess->state() != QProcess::NotRunning) {
        serverProcess->terminate();
    } else {
        serverProcess->start(serverName, arguments);
    }
}

void MainWindow::onServerError()
{
    textEdit->setText(serverProcess->errorString());
}

void MainWindow::onServerStateChanged()
{
    switch (serverProcess->state()) {
    case QProcess::Running:
        startServerButton->setText("(running) Stop input method server");
        break;
    case QProcess::Starting:
        startServerButton->setText("(starting) Stop input method server");
        break;
    case QProcess::NotRunning:
        startServerButton->setText("(stopped) Start input method server");
    default:
        break;
    }
}

void MainWindow::onRotateKeyboardClicked()
{
    const Maliit::OrientationAngle orientations[] = {Maliit::Angle0,
                                                     Maliit::Angle90,
                                                     Maliit::Angle180,
                                                     Maliit::Angle270};
    orientationIndex++;
    if (orientationIndex >= 4) {
        orientationIndex = 0;
    }
    const Maliit::OrientationAngle angle = orientations[orientationIndex];

    //! [Get singleton InputMethod instance]
    Maliit::InputMethod::instance()->setOrientationAngle(angle);
    //! [Get singleton InputMethod instance]
}
