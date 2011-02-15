#include "ft_mimsettingsapplet.h"

#ifdef HAVE_MEEGOTOUCH
#include <dcpappletmetadata.h>
#include <dcpappletplugin.h>
#endif

#include <QFileInfo>
#include <QDebug>

void ft_MImSettingsApplet::testAppletIsLoadable()
{
#ifdef HAVE_MEEGOTOUCH
    // stub_dcpappletmetadata.cpp gives the real applet file.
    DcpAppletMetadata metadata("/dev/null");
    qDebug()<< "Checking lib at:" <<  metadata.fullBinary();

    // Currently, the settings applet could be missing due to the integration issue.
    // So skip the test if applet is not available.
    if (!QFileInfo(metadata.fullBinary()).exists()) {
        QSKIP("Text Input setting applet is not available, please check the lib file",
              SkipAll);
    }

    DcpAppletPlugin loader(&metadata);
    QVERIFY(loader.applet());
#endif
}

QTEST_APPLESS_MAIN(ft_MImSettingsApplet)
