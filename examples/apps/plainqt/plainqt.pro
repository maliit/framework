TEMPLATE = app
TARGET = maliit-exampleapp-plainqt
target.path = $${M_IM_INSTALL_BIN}
DEPENDPATH += .

SOURCES += \
           plainqt.cpp \
           mainwindow.cpp \

HEADERS += \
           mainwindow.h \

QT += core gui

INSTALLS += target
QMAKE_CLEAN += maliit-exampleapp-plainqt
