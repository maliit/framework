# Directories (taken from libmeegotouch)
unix {
     M_PREFIX = /usr
     M_INSTALL_BIN = $$M_PREFIX/bin
     M_INSTALL_LIBS = $$M_PREFIX/lib
     M_INSTALL_HEADERS = $$M_PREFIX/include/meegotouch
     M_THEME_DIR = $$M_PREFIX/share/themes
     M_TRANSLATION_DIR = $$M_PREFIX/share/l10n/meegotouch
     M_APPLET_DIR = $$M_PREFIX/lib/meegotouch/applets/
     M_APPLET_DATA_DIR = $$M_PREFIX/share/meegotouch/applets
     M_THEME_PRELOAD_DIR = /var/lib/meegotouch/theme/preload.d
     M_THEME_POST_PRELOAD_DIR = /var/lib/meegotouch/theme/preload.post
     M_APPLET_SETTINGS_DIR= $$M_PREFIX/share/meegotouch/applets/settings
}
mac {
     # Do mac stuff here
    M_BUILD_FEATURES = debug
    M_PROFILE_PARTS =
    INCLUDEPATH += include/
    M_INSTALL_BIN = $$M_PREFIX/bin/
    M_INSTALL_LIBS = $$M_PREFIX/lib/
    M_INSTALL_HEADERS = $$M_PREFIX/include/meegotouch/
    M_THEME_DIR = $$M_PREFIX/share/themes
    M_TRANSLATION_DIR = $$M_PREFIX/share/l10n/meegotouch/
    M_APPLET_DIR = $$M_PREFIX/lib/meegotouch/applets/
    M_APPLET_DATA_DIR = $$M_PREFIX/share/meegotouch/applets/
    M_APPLET_SETTINGS_DIR= $$M_PREFIX/share/meegotouch/applets/settings/
}
win32 {
     # Do win32 stuff here
     M_PREFIX = /usr
     M_INSTALL_BIN = $$M_PREFIX/bin/
     M_INSTALL_LIBS = $$M_PREFIX/lib/
     M_INSTALL_HEADERS = $$M_PREFIX/include/meegotouch/
     M_THEME_DIR = $$M_PREFIX/share/themes
     M_TRANSLATION_DIR = $$M_PREFIX/share/l10n/meegotouch/
     M_APPLET_DIR = $$M_PREFIX/lib/meegotouch/applets/
     M_APPLET_DATA_DIR = $$M_PREFIX/share/meegotouch/applets/
}
