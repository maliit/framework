include(../common_top.pri)

!nomeegotouch:!noduicontrolpanel {
    DEFINES += HAVE_MEEGOTOUCH
    CONFIG += meegotouch duicontrolpanel
} else {
    DEFINES -= HAVE_MEEGOTOUCH
}

# Input
HEADERS += ft_mimsettingsapplet.h \

SOURCES += ft_mimsettingsapplet.cpp \
           stub_dcpappletmetadata.cpp \

TARGET = ft_mimsettingsapplet

CONFIG += console
CONFIG -= gui windows

include(../common_check.pri)

