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

} // namespace

class MyTextEdit
    : public QTextEdit
{
private:
    bool was_focused;

public:
    MyTextEdit()
        : QTextEdit(TextPrompt)
        , was_focused(false)
    {}

protected:
    void focusInEvent(QFocusEvent *e)
    {
        toPlainText();
        // On first text edit, clear pre-set TextPrompt:
        if (not was_focused && toPlainText() == QString(TextPrompt)) {
            was_focused = true;
            setText("");
        }

        QTextEdit::focusInEvent(e);
    }
};

MainWindow::MainWindow()
    : QMainWindow()
    , m_server_process(new QProcess(this))
    , m_orientation_index(0)
    , m_start_server(new QPushButton)
    , m_rotate_keyboard(new QPushButton("Rotate input method"))
    , m_entry(new QTextEdit)
{
    m_server_process->setProcessChannelMode(QProcess::ForwardedChannels);

    connect(m_server_process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(onServerError()));
    connect(m_server_process, SIGNAL(stateChanged(QProcess::ProcessState)),
            this, SLOT(onServerStateChanged()));

    connect(m_start_server, SIGNAL(clicked()),
            this, SLOT(onStartServerClicked()));
    connect(m_rotate_keyboard, SIGNAL(clicked()),
            this, SLOT(onRotateKeyboardClicked()));

    initUI();
    onServerStateChanged();
}

void MainWindow::initUI()
{
    setWindowTitle("Maliit test application");

    QGridLayout *grid = new QGridLayout;
    int row = 0;

    QHBoxLayout *buttons = new QHBoxLayout;
    buttons->addWidget(m_start_server);
    buttons->addWidget(m_rotate_keyboard);
    // Clicking the button will steal focus from the text edit, thus hiding
    // the virtual keyboard:
    buttons->addWidget(new QPushButton("Dismiss input method"));

    grid->addLayout(buttons, row, 1);
    ++row;

    grid->addWidget(m_entry, row, 1);
    ++row;

    // Don't want other buttons to steal focus:
    m_start_server->setFocusProxy(m_entry);
    m_rotate_keyboard->setFocusProxy(m_entry);

    QPushButton *close_app = new QPushButton("Close application");
    grid->addWidget(close_app, row, 1);
    ++row;

    connect(close_app, SIGNAL(clicked()),
            this,     SLOT(close()));

    setCentralWidget(new QWidget);
    centralWidget()->setLayout(grid);

    if (enableFullScreenMode()) {
        showFullScreen();
    } else {
        show();
    }
}

MainWindow::~MainWindow()
{
    m_server_process->terminate();
}

void MainWindow::onStartServerClicked()
{

    QStringList arguments;
    arguments << "-bypass-wm-hint";

    // Self-compositing is currently only supported in fullscreen mode:
    if (enableFullScreenMode()) {
        arguments << "-use-self-composition";
    }

    if (m_server_process->state() != QProcess::NotRunning) {
        m_server_process->terminate();
    } else {
        m_server_process->start(serverName, arguments);
    }
}

void MainWindow::onServerError()
{
    m_entry->setText(m_server_process->errorString());
}

void MainWindow::onServerStateChanged()
{
    switch (m_server_process->state()) {
    case QProcess::Running:
        m_start_server->setText("(running) Stop input method server");
        break;

    case QProcess::Starting:
        m_start_server->setText("(starting) Stop input method server");
        break;

    case QProcess::NotRunning:
        m_start_server->setText("(stopped) Start input method server");
        break;

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
    m_orientation_index++;
    if (m_orientation_index >= 4) {
        m_orientation_index = 0;
    }
    const Maliit::OrientationAngle angle = orientations[m_orientation_index];
    Maliit::InputMethod::instance()->setOrientationAngle(angle);
}
