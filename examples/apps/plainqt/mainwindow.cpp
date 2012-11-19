#include "mainwindow.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QGuiApplication>
#include <QScreen>
#include <QWindow>
#endif

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
    , m_grid_row(0)
    , m_grid(new QGridLayout)
    , m_start_server(new QPushButton)
    , m_rotate_keyboard(new QPushButton("Rotate input method"))
{
    m_server_process->setProcessChannelMode(QProcess::ForwardedChannels);

    connect(m_server_process, SIGNAL(stateChanged(QProcess::ProcessState)),
            this, SLOT(onServerStateChanged()));

    connect(m_start_server, SIGNAL(clicked()),
            this, SLOT(onStartServerClicked()));
    connect(m_rotate_keyboard, SIGNAL(clicked()),
            this, SLOT(onRotateKeyboardClicked()));

    initUI();
    onServerStateChanged();

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    // Work around a bug in maliit input method support where primary orientation is always portrait
    windowHandle()->reportContentOrientationChange(windowHandle()->screen()->primaryOrientation());
#endif
}

void MainWindow::initUI()
{
    setWindowTitle("Maliit test application");
    m_grid_row = 0;

    QHBoxLayout *buttons = new QHBoxLayout;
    buttons->addWidget(m_start_server);
    buttons->addWidget(m_rotate_keyboard);
    // Clicking the button will steal focus from the text edit, thus hiding
    // the virtual keyboard:
    buttons->addWidget(new QPushButton("Dismiss input method"));

    m_grid->addLayout(buttons, m_grid_row, 1);
    ++m_grid_row;

    // multi line:
    QLabel *label = 0;
    QTextEdit *entry = 0;

    m_grid->addWidget(label = new QLabel("multi line"), m_grid_row, 0);
    m_grid->addWidget(entry = new QTextEdit, m_grid_row, 1);
    ++m_grid_row;

    label->setToolTip("Qt::ImhNone");
    entry->setInputMethodHints(Qt::ImhNone);
    entry->installEventFilter(this);

    // single line, emulating content types via Qt::InputMethodHints:
    insertIntoGrid("single line", Qt::ImhNone,
                   "Qt::ImhNone");
    insertIntoGrid("password", Qt::ImhHiddenText|Qt::ImhNoPredictiveText,
                   "Qt::ImhHiddenText|Qt::ImhNoPredictiveText");
    insertIntoGrid("numbers only", Qt::ImhFormattedNumbersOnly,
                   "Qt::ImhFormattedNumbersOnly");
    insertIntoGrid("dialer input", Qt::ImhDialableCharactersOnly,
                   "Qt::ImhDialableCharactersOnly");
    insertIntoGrid("symbol view", Qt::ImhPreferNumbers,
                   "Qt::ImhPreferNumbers");
    insertIntoGrid("e-mail", Qt::ImhEmailCharactersOnly,
                   "Qt::ImhEmailCharactersOnly");
    insertIntoGrid("website", Qt::ImhUrlCharactersOnly,
                   "Qt::ImhUrlCharactersOnly");

    // Don't want other buttons to steal focus:
    m_start_server->setFocusPolicy(Qt::NoFocus);
    m_rotate_keyboard->setFocusPolicy(Qt::NoFocus);

    QPushButton *close_app = new QPushButton("Close application");
    m_grid->addWidget(close_app, m_grid_row, 1);
    ++m_grid_row;

    connect(close_app, SIGNAL(clicked()),
            this,     SLOT(close()));

    setCentralWidget(new QWidget);
    centralWidget()->setLayout(m_grid);

    if (enableFullScreenMode()) {
        showFullScreen();
    } else {
        show();
    }
}

void MainWindow::insertIntoGrid(const QString &description,
                                const Qt::InputMethodHints &hints,
                                const QString &tooltip)
{
    QLabel *label = 0;
    QLineEdit *entry = 0;

    m_grid->addWidget(label = new QLabel(description), m_grid_row, 0);
    m_grid->addWidget(entry = new QLineEdit, m_grid_row, 1);
    ++m_grid_row;

    label->setToolTip(tooltip);
    entry->setInputMethodHints(hints);
    entry->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    m_server_process->terminate();
}

bool MainWindow::eventFilter(QObject *watched,
                             QEvent *event)
{
    Q_UNUSED(watched)

    // Let the input method show up on focus-in, not on second click:
    if (event->type() == QFocusEvent::FocusIn) {
#if QT_VERSION >= 0x050000
        qApp->inputMethod()->show();
#else
        if (QInputContext *ic = qApp->inputContext()) {
            QEvent im_request(QEvent::RequestSoftwareInputPanel);
            ic->filterEvent(&im_request);
        }
#endif
    }

    return false;
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
    ++m_orientation_index;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    static const Qt::ScreenOrientation orientations[] = {
        Qt::LandscapeOrientation,
        Qt::PortraitOrientation,
        Qt::InvertedLandscapeOrientation,
        Qt::InvertedPortraitOrientation
    };

    windowHandle()->reportContentOrientationChange(orientations[m_orientation_index % 4]);
#else
    static const const Maliit::OrientationAngle orientations[] = {
        Maliit::Angle0,
        Maliit::Angle90,
        Maliit::Angle180,
        Maliit::Angle270
    };

    Maliit::InputMethod::instance()->setOrientationAngle(orientations[m_orientation_index % 4]);
#endif
}
