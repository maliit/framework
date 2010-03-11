include(../duiconfig.pri)

VERSION = 0.1.0
TEMPLATE = lib
TARGET=duiimframework

# Input
HEADERSINSTALL = \
    duiinputmethodplugin.h \
    duiinputmethodbase.h \
    duiimpluginmanager.h \
    duiinputcontextconnection.h \
    duiplainwindow.h \
    duipreeditface.h \
    duiimhandlerstate.h \

HEADERS += $$HEADERSINSTALL \
        duiimpluginmanager_p.h \
        duiinputcontextdbusconnection.h \
        duiinputcontextdbusconnection_p.h \


SOURCES += duiimpluginmanager.cpp \
        duiinputmethodbase.cpp \
        duiinputcontextconnection.cpp \
        duiinputcontextdbusconnection.cpp \
        duiplainwindow.cpp \
        duiimpluginmanager_p.cpp \

CONFIG += debug qdbus dui
QT = core gui network

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

QMAKE_CLEAN += *.gcno *.gcda

target.path += $$DUI_INSTALL_LIBS

OBJECTS_DIR = .obj
MOC_DIR = .moc

headers.path += $$DUI_INSTALL_HEADERS/duiimframework
headers.files += $$HEADERSINSTALL

install_headers.path = $$DUI_INSTALL_HEADERS/duiimframework 
install_headers.files = include/*

install_pkgconfig.path = $$[QT_INSTALL_LIBS]/pkgconfig
install_pkgconfig.files = DuiImUiServer.pc

install_prf.path = $$[QT_INSTALL_DATA]/mkspecs/features
install_prf.files = duiimframework.prf

INSTALLS += target \
    headers \
    install_headers \
    install_pkgconfig \
    install_prf \

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.depends += lib$${TARGET}.so.$${VERSION}

QMAKE_EXTRA_TARGETS += check
check.target = check
check.depends += lib$${TARGET}.so.$${VERSION}

contains(DEFINES, DUI_IM_DISABLE_TRANSLUCENCY) {
    DUI_IM_FRAMEWORK_FEATURE += DUI_IM_DISABLE_TRANSLUCENCY
} else {
    DUI_IM_FRAMEWORK_FEATURE -= DUI_IM_DISABLE_TRANSLUCENCY
}

PRF_FILE = duiimframework.prf.in

prffilegenerator.output = duiimframework.prf
prffilegenerator.input = PRF_FILE
prffilegenerator.commands += sed -e \"s:DUI_IM_FRAMEWORK_FEATURE:$$DUI_IM_FRAMEWORK_FEATURE:g\" ${QMAKE_FILE_NAME} > ${QMAKE_FILE_OUT}
prffilegenerator.CONFIG = target_predeps no_link
prffilegenerator.depends += Makefile
QMAKE_EXTRA_COMPILERS += prffilegenerator

