include(../common_top.pri)

# Input
HEADERS += ft_mimsettingsapplet.h \

SOURCES += ft_mimsettingsapplet.cpp \
           stub_dcpappletmetadata.cpp \

TARGET = ft_mimsettingsapplet

CONFIG += duicontrolpanel meegotouch console
CONFIG -= gui windows

include(../common_check.pri)

