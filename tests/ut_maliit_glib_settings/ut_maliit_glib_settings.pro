include(../common_top.pri)

CONFIG += link_pkgconfig
PKGCONFIG += gio-unix-2.0

SOURCES += \
    ut_maliit_glib_settings.c  \
    mockmaliitserver.c \

HEADERS += \
    mockmaliitserver.h

include ($$TOP_DIR/maliit-glib/libmaliit-glib.pri)

include(../common_check.pri)
