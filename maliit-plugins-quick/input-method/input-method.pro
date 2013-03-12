include(../../config.pri)

TOP_DIR = ../..

VERSION = $$MALIIT_ABI_VERSION
TEMPLATE = lib
TARGET = $$TOP_DIR/lib/$$MALIIT_PLUGINS_QUICK_LIB

include($$TOP_DIR/src/libmaliit-plugins.pri)
include($$TOP_DIR/common/libmaliit-common.pri)

# Input
HEADERSINSTALL = \
        minputmethodquickplugin.h \
        minputmethodquick.h \
        maliitquick.h \
        mkeyoverridequick.h

HEADERS += $$HEADERSINSTALL \
        mkeyoverridequick_p.h \
        tooltip_p.h

SOURCES += \
        minputmethodquickplugin.cpp \
        minputmethodquick.cpp \
        mkeyoverridequick.cpp \
        tooltip.cpp

QT = core gui gui-private qml quick

MOC_DIR = .moc

CONFIG += link_pkgconfig

PKGCONFIG += xcb xcb-xfixes

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

target.path += $$LIBDIR

QMAKE_CLEAN += $$OBJECTS_DIR/*.gcno $$OBJECTS_DIR/*.gcda

headers.path += $$INCLUDEDIR/$$MALIIT_PLUGINS_QUICK_HEADER
headers.files += $$HEADERSINSTALL

outputFiles(maliit-plugins-quick.pc)

QT_PRF_DIR = $$[QT_INSTALL_DATA]/mkspecs/features
QT_PREFIX = $$[QT_INSTALL_PREFIX]
local-install {
    QT_PRF_DIR = $$replace(QT_PRF_DIR, $$QT_PREFIX, $$PREFIX)
}

install_pkgconfig.path = $${LIBDIR}/pkgconfig
install_pkgconfig.files = $$OUT_PWD/maliit-plugins-quick.pc

INSTALLS += target \
    headers \
    install_pkgconfig \

