#include "mainwindow.h"

#include <maliit/inputmethod.h>
#include <maliit/namespace.h>

#include <cstdlib>

#include <QApplication>
#include <QInputContext>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QVariant>
#include <QVBoxLayout>
#include <QX11Info>

namespace {
    const char * const FillerText = "Mauris pretium vehicula tellus, vitae "
            "tristique sapien malesuada a. Mauris vel ipsum ante. Donec "
            "faucibus quam sit amet neque rhoncus sit amet egestas eros "
            "ullamcorper. Integer laoreet augue eget dui viverra et pharetra "
            "dui porttitor. Donec sodales venenatis sollicitudin. Sed sodales "
            "lacinia ligula, at fringilla lacus adipiscing in. Nam purus nisi,"
            " pulvinar in tristique eget, tincidunt eu ante. Donec eros eros, "
            "molestie nec pellentesque ut, imperdiet sed metus. Quisque et "
            "erat ante. Cum sociis natoque penatibus et magnis dis parturient "
            "montes, nascetur ridiculus mus. Pellentesque faucibus consectetur "
            "tortor sed accumsan. Mauris id metus felis. Aenean at volutpat "
            "nunc. Pellentesque habitant morbi tristique senectus et netus et "
            "malesuada fames ac turpis egestas. Suspendisse non ultricies "
            "turpis.";

    bool enableFullScreenMode()
    {
        static const bool fullscreen(qApp->arguments().contains("-fullscreen"));
        return fullscreen;
    }
}

MainWindow::MainWindow()
    : QMainWindow()
    , entryWithProperties(new QTextEdit)
{
    setWindowTitle("Maliit widget properties test application");

    QVBoxLayout *vbox = new QVBoxLayout;

    // Clicking the button will steal focus from the text edit, thus hiding
    // the virtual keyboard:
    QPushButton *hideVkb = new QPushButton("Hide virtual keyboard");
    vbox->addWidget(hideVkb);

    QCheckBox *translucencyCheckBox(new QCheckBox("Make input method translucent"));
    translucencyCheckBox->setFocusProxy(entryWithProperties);

    QCheckBox *preferNumbersCheckBox(new QCheckBox("Prefer numbers (show symview first)"));
    preferNumbersCheckBox->setFocusProxy(entryWithProperties);

    QTextEdit *hiddenText(new QTextEdit);
    hiddenText->setTextColor(Qt::gray);
    hiddenText->setFontPointSize(16);
    hiddenText->setText(FillerText);

    vbox->addWidget(new QLabel("Requires Harmattan keyboard plugin to show its "
                               "impact, sorry!"));
    vbox->addWidget(new QLabel("Regular line edit"));
    vbox->addWidget(new QLineEdit);
    vbox->addWidget(entryWithProperties);
    vbox->addWidget(translucencyCheckBox);
    vbox->addWidget(preferNumbersCheckBox);
    vbox->addStretch();
    vbox->addWidget(hiddenText);

    connect(translucencyCheckBox, SIGNAL(toggled(bool)),
            this,                 SLOT(onTranslucencyToggled(bool)));

    connect(preferNumbersCheckBox, SIGNAL(toggled(bool)),
            this,                  SLOT(onPreferNumbersToggled(bool)));

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

void MainWindow::onTranslucencyToggled(bool value)
{
    entryWithProperties->setProperty(Maliit::InputMethodQuery::translucentInputMethod, QVariant(value));
    if (QInputContext *ic = qApp->inputContext()) {
        ic->update();
    }
}

void MainWindow::onPreferNumbersToggled(bool value)
{
    entryWithProperties->setInputMethodHints(value ? (entryWithProperties->inputMethodHints() | Qt::ImhPreferNumbers)
                                                   : (entryWithProperties->inputMethodHints() & ~Qt::ImhPreferNumbers));
    if (QInputContext *ic = qApp->inputContext()) {
        ic->update();
    }
}
