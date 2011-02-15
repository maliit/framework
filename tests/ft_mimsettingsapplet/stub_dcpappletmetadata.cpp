#ifdef HAVE_MEEGOTOUCH
#include <dcpappletmetadata.h>
#include <QFileInfo>

/* This is used with appletloader for checking if the applet is
 * loadable. */

QString DcpAppletMetadata::fullBinary() const
{
    QString filename = "libmimsettingsapplet.so";

    // for the local version (without install)
    QString path = "../../settings-applet/" + filename;
    if (QFileInfo(path).exists()) {
        return path;
    } else {
        // installed version of the test
        return QString("/usr/lib/duicontrolpanel/applets/" + filename);
    }
}
#endif
