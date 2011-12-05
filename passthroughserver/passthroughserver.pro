include(../config.pri)

TEMPLATE = app
TARGET = $$MALIIT_SERVER
target.path = $$M_IM_INSTALL_BIN
DEPENDPATH += .
INCLUDEPATH += . .. ../src ../common

LIBS += ../src/lib$${MALIIT_PLUGINS_LIB}.so -lXfixes

SOURCES += main.cpp

QT += core $$QT_WIDGETS

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

DBUS_SERVICES_DIR = $$system(pkg-config --variable session_bus_services_dir dbus-1)
DBUS_SERVICES_PREFIX = $$system(pkg-config --variable prefix dbus-1)
enforce-install-prefix {
    DBUS_SERVICES_DIR = $$replace(DBUS_SERVICES_DIR, $$DBUS_SERVICES_PREFIX, $$M_IM_PREFIX)
}
servicefiles.path = $$DBUS_SERVICES_DIR
servicefiles.files = minputmethodserver.service
servicefiles.CONFIG += no_check_exist
