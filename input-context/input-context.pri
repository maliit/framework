include(../config.pri)

TEMPLATE = lib
DEPENDPATH += ..

OBJECTS_DIR = .obj
MOC_DIR = .moc

include(../common/libmaliit-common.pri)
include(../maliit/libmaliit.pri)
include(../connection/libmaliit-connection.pri)

DEFINES += MALIIT_INPUTCONTEXT_NAME=\\\"$${MALIIT_INPUTCONTEXT_NAME}\\\"

QT = core gui
CONFIG += link_pkgconfig

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

QMAKE_CLEAN += $$OBJECTS_DIR/*.gcno $$OBJECTS_DIR/*.gcda

QT_IM_PLUGIN_PATH = $$[QT_INSTALL_PLUGINS]/inputmethods
QT_PREFIX = $$[QT_INSTALL_PREFIX]
local-install {
    QT_IM_PLUGIN_PATH = $$replace(QT_IM_PLUGIN_PATH, $$QT_PREFIX, $$PREFIX)
}
