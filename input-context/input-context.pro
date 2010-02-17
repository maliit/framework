TEMPLATE = lib
TARGET = duiinputcontext
DEPENDPATH += .
INCLUDEPATH += . ../src

OBJECTS_DIR = .obj
MOC_DIR = .moc

STYLE_HEADERS += \
    duipreeditstyle.h \

# Input
HEADERS += duiinputcontext.h duiinputcontextplugin.h duiinputcontextadaptor.h $$STYLE_HEADERS
SOURCES += duiinputcontext.cpp duiinputcontextplugin.cpp duiinputcontextadaptor.cpp

QT = core gui
CONFIG += plugin debug qdbus dui

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

QMAKE_CLEAN += *.gcno *.gcda

target.path += $$[QT_INSTALL_PLUGINS]/inputmethods
INSTALLS    += target \

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.depends += lib$${TARGET}.so

QMAKE_EXTRA_TARGETS += check
check.target = check
check.depends += lib$${TARGET}.so

