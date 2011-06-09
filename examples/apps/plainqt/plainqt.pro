TEMPLATE = app
TARGET = maliit-exampleapp-plainqt
target.path = /usr/bin
DEPENDPATH += .

SOURCES += \
           plainqt.cpp \
           mainwindow.cpp \

HEADERS += \
           mainwindow.h \

QT += core gui

INSTALLS += target
QMAKE_CLEAN += maliit-exampleapp-plainqt
