OBJECTS_DIR = .obj
MOC_DIR = .moc

# Input
HEADERS += ft_mimsettingsapplet.h \

SOURCES += ft_mimsettingsapplet.cpp \
           stub_dcpappletmetadata.cpp \

TARGET = ft_mimsettingsapplet

CONFIG += duicontrolpanel meegotouch console
CONFIG -= gui windows

include(../common_check.pri)

