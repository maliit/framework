include(../config.pri)

TEMPLATE = app
TARGET = $$MALIIT_SERVER
target.path = $$M_IM_INSTALL_BIN
DEPENDPATH += .
INCLUDEPATH += . ../src

LIBS += ../src/lib$${MALIIT_PLUGINS_LIB}.so -lXfixes -lX11

SOURCES += \
           main.cpp \
           mpassthruwindow.cpp \
           mimdummyinputcontext.cpp

HEADERS += \
           mpassthruwindow.h \
           mimdummyinputcontext.h

QT += core gui

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

QMAKE_CLEAN += *.gcno *.gcda

INSTALLS += target

# installing dbus service file needs to be explicitly enabled
dbusservice {
   INSTALLS += servicefiles
}


QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.depends += $$TARGET

QMAKE_EXTRA_TARGETS += check
check.target = check
check.depends += $$TARGET

system(dpkg-architecture -ei386):SOFTWARE="-software -bypass-wm-hint "
message(Argument added to im server: $$SOFTWARE $$TARGETDEVICE)

SERVICE_FILE = minputmethodserver.service.in

ACTIVATION_OR_REAL_INTERFACE = com.meego.inputmethod.uiserveractivation

servicefilegenerator.output = minputmethodserver.service
servicefilegenerator.input = SERVICE_FILE
servicefilegenerator.commands += sed -e \"s:M_BIN_PATH:$$M_IM_INSTALL_BIN:g\" -e \"s:SOFTWARE:$$SOFTWARE:g\" -e \"s:TARGET:$$TARGETDEVICE:g\" -e \"s:ACTIVATION_OR_REAL_INTERFACE:$$ACTIVATION_OR_REAL_INTERFACE:g\" -e \"s:MALIIT_SERVER:$$MALIIT_SERVER:g\" ${QMAKE_FILE_NAME} > ${QMAKE_FILE_OUT}
servicefilegenerator.CONFIG = target_predeps no_link
QMAKE_EXTRA_COMPILERS += servicefilegenerator

servicefiles.path = $$system(pkg-config --variable session_bus_services_dir dbus-1)
servicefiles.files = minputmethodserver.service
servicefiles.CONFIG += no_check_exist
