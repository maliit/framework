include(../../../config.pri)

TEMPLATE = app
TARGET = maliit-exampleapp-plainqt
target.path = /usr/bin
DEPENDPATH += .

TOP_SRC = ../../..
INCLUDEPATH += $$TOP_SRC
LIBS += $$TOP_SRC/maliit/lib$${MALIIT_LIB}.so

SOURCES += \
           plainqt.cpp \
           mainwindow.cpp \

HEADERS += \
           mainwindow.h \

QT += core gui

INSTALLS += target
QMAKE_CLEAN += maliit-exampleapp-plainqt
