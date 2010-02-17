# Directories (taken from libdui)
unix {
     DUI_PREFIX = /usr
     DUI_INSTALL_BIN = $$DUI_PREFIX/bin
     DUI_INSTALL_LIBS = $$DUI_PREFIX/lib
     DUI_INSTALL_HEADERS = $$DUI_PREFIX/include/dui
     DUI_THEME_DIR = $$DUI_PREFIX/share/themes
     DUI_TRANSLATION_DIR = $$DUI_PREFIX/share/l10n/dui
     DUI_APPLET_DIR = $$DUI_PREFIX/lib/dui/applets/
     DUI_APPLET_DATA_DIR = $$DUI_PREFIX/share/dui/applets
     DUI_THEME_PRELOAD_DIR = /var/lib/dui/theme/preload.d
     DUI_THEME_POST_PRELOAD_DIR = /var/lib/dui/theme/preload.post
     DUI_APPLET_SETTINGS_DIR= $$DUI_PREFIX/share/dui/applets/settings
}
mac {
     # Do mac stuff here
    DUI_BUILD_FEATURES = debug
    DUI_PROFILE_PARTS =
    INCLUDEPATH += include/
    DUI_INSTALL_BIN = $$DUI_PREFIX/bin/
    DUI_INSTALL_LIBS = $$DUI_PREFIX/lib/
    DUI_INSTALL_HEADERS = $$DUI_PREFIX/include/dui/
    DUI_THEME_DIR = $$DUI_PREFIX/share/themes
    DUI_TRANSLATION_DIR = $$DUI_PREFIX/share/l10n/dui/
    DUI_APPLET_DIR = $$DUI_PREFIX/lib/dui/applets/
    DUI_APPLET_DATA_DIR = $$DUI_PREFIX/share/dui/applets/
    DUI_APPLET_SETTINGS_DIR= $$DUI_PREFIX/share/dui/applets/settings/
}
win32 {
     # Do win32 stuff here
     DUI_PREFIX = /usr
     DUI_INSTALL_BIN = $$DUI_PREFIX/bin/
     DUI_INSTALL_LIBS = $$DUI_PREFIX/lib/
     DUI_INSTALL_HEADERS = $$DUI_PREFIX/include/dui/
     DUI_THEME_DIR = $$DUI_PREFIX/share/themes
     DUI_TRANSLATION_DIR = $$DUI_PREFIX/share/l10n/dui/
     DUI_APPLET_DIR = $$DUI_PREFIX/lib/dui/applets/
     DUI_APPLET_DATA_DIR = $$DUI_PREFIX/share/dui/applets/
}
