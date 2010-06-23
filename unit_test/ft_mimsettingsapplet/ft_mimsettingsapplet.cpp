#include "ft_mimsettingsapplet.h"
#include <dcpappletmetadata.h>
#include <dcpappletplugin.h>

void ft_MImSettingsApplet::testAppletIsLoadable()
{
    DcpAppletMetadata metadata("/dev/null");
    printf("Checking lib at %s \n", qPrintable(metadata.fullBinary()));
    DcpAppletPlugin loader(&metadata);
    QVERIFY(loader.applet());
}

QTEST_APPLESS_MAIN(ft_MImSettingsApplet)

