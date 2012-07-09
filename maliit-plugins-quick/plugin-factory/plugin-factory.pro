include(../../config.pri)

TOP_DIR = ../..

TEMPLATE = lib
TARGET = $$TOP_DIR/lib/plugins/$$MALIIT_PLUGINS_QUICK_FACTORY

include($$TOP_DIR/common/libmaliit-common.pri)
include($$TOP_DIR/maliit-plugins-quick/libmaliit-plugins-quick.pri)
include($$TOP_DIR/src/libmaliit-plugins.pri)

CONFIG += plugin

# Input
HEADERS += \
        maliitquickpluginfactory.h \

SOURCES += \
        maliitquickpluginfactory.cpp \

QT = core $$QT_WIDGETS declarative

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

target.path += $${MALIIT_FACTORY_PLUGINS_DIR}

OBJECTS_DIR = .obj
MOC_DIR = .moc

QMAKE_CLEAN += $$OBJECTS_DIR/*.gcno $$OBJECTS_DIR/*.gcda

INSTALLS += target
