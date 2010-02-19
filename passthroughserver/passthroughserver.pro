include(../duiconfig.pri)

TEMPLATE = app
TARGET = dui-im-uiserver
target.path = $$DUI_INSTALL_BIN
DEPENDPATH += .
INCLUDEPATH += . ../src
CONFIG += duireactionmap

LIBS += ../src/libduiimframework.so -lXfixes

SOURCES += \
           main.cpp \
           duipassthruwindow.cpp \

HEADERS += \
           duipassthruwindow.h \

QT += opengl core gui
CONFIG += debug dui

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

QMAKE_CLEAN += *.gcno *.gcda

INSTALLS += target \
         servicefiles

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.depends += $$TARGET

QMAKE_EXTRA_TARGETS += check 
check.target = check
check.depends += $$TARGET

system(dpkg-architecture -ei386):SOFTWARE="-software -bypass-wm-hint "
message(Argument added to im server: $$SOFTWARE)

SERVICE_FILE = duiinputmethodserver.service.in

servicefilegenerator.output = duiinputmethodserver.service
servicefilegenerator.input = SERVICE_FILE
servicefilegenerator.commands += sed -e \"s:DUI_BIN_PATH:$$DUI_INSTALL_BIN:g\" -e \"s:SOFTWARE:$$SOFTWARE:g\" ${QMAKE_FILE_NAME} > ${QMAKE_FILE_OUT}
servicefilegenerator.CONFIG = target_predeps no_link
QMAKE_EXTRA_COMPILERS += servicefilegenerator

servicefiles.path = $$system(pkg-config --variable session_bus_services_dir dbus-1)
servicefiles.files = duiinputmethodserver.service
