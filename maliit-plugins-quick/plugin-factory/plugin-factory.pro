include(../../config.pri)

TEMPLATE = lib
TARGET = $$MALIIT_PLUGINS_QUICK_FACTORY
QUICK_DIR = $${OUT_PWD}/../input-method

INCLUDEPATH_BASE = ../..
INCLUDEPATH += \
        $${INCLUDEPATH_BASE} \
        $${INCLUDEPATH_BASE}/src \
        $${INCLUDEPATH_BASE}/common \

CONFIG += plugin

# Input
HEADERS += \
        maliitquickpluginfactory.h \

SOURCES += \
        maliitquickpluginfactory.cpp \

LIBS += \
        $$QUICK_DIR/lib$${MALIIT_PLUGINS_QUICK_LIB}.so \
        $${OUT_PWD}/../../src/lib$${MALIIT_PLUGINS_LIB}.so \

QT = core $$QT_WIDGETS declarative

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

target.path += $${M_IM_FACTORY_PLUGINS_DIR}

OBJECTS_DIR = .obj
MOC_DIR = .moc

QMAKE_CLEAN += $$OBJECTS_DIR/*.gcno $$OBJECTS_DIR/*.gcda

INSTALLS += target
