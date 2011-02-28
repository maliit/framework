include(../common_top.pri)

PASSTHRU_DIR = ../../passthroughserver
INCLUDEPATH += \
    $$SRC_DIR \
    $$PASSTHRU_DIR \
    
DEFINES += UNIT_TEST

# Input
HEADERS += \
    ut_selfcompositing.h \
    $$SRC_DIR/mimapplication.h \
    $$SRC_DIR/mimremotewindow.h \
    $$SRC_DIR/mimwidget.h \ 
    $$SRC_DIR/mimgraphicsview.h \
    $$PASSTHRU_DIR/mpassthruwindow.h \

SOURCES += \
    ut_selfcompositing.cpp \
    $$SRC_DIR/mimapplication.cpp \
    $$SRC_DIR/mimremotewindow.cpp \ 
    $$SRC_DIR/mimwidget.cpp \ 
    $$SRC_DIR/mimgraphicsview.cpp \
    $$PASSTHRU_DIR/mpassthruwindow.cpp \

LIBS += \
    $$SRC_DIR/libmeegoimframework.so.0

include(../common_check.pri)
