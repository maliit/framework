include(../input-context.pri)

TARGET = $$MALIIT_INPUTCONTEXTPLUGIN_TARGETNAME

INCLUDEPATH += ../maliit-input-context
LIBS += ../maliit-input-context/lib$${MALIIT_INPUTCONTEXT_TARGETNAME}.a

# Input
HEADERS += minputcontextplugin.h \

SOURCES += minputcontextplugin.cpp \

CONFIG += plugin

PKGCONFIG += dbus-glib-1
QT += dbus
system(pkg-config gio-2.0 --atleast-version 2.26) {
    DEFINES   += MALIIT_USE_GIO_API
    PKGCONFIG += gio-2.0
}

INSTALLS    += target
target.path += $$QT_IM_PLUGIN_PATH
