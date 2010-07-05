#include "ft_mimsettingsapplet.h"
#include <dcpappletmetadata.h>
#include <dcpappletplugin.h>
#include <QFileInfo>
#include <QDebug>

void ft_MImSettingsApplet::testAppletIsLoadable()
{
    DcpAppletMetadata metadata("/dev/null");
    qDebug()<< "Checking lib at:" <<  qPrintable(metadata.fullBinary());

    if (!QFileInfo(metadata.fullBinary()).exists()) {
        QSKIP("Text Input setting applet is not available, please check the lib file",
              SkipAll);
    }

    DcpAppletPlugin loader(&metadata);
    QVERIFY(loader.applet());
}

QTEST_APPLESS_MAIN(ft_MImSettingsApplet)

