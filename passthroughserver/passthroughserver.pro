include(../mconfig.pri)

TEMPLATE = app
TARGET = meego-im-uiserver
target.path = $$M_INSTALL_BIN
DEPENDPATH += .
INCLUDEPATH += . ../src
CONFIG += duireactionmap

LIBS += ../src/libmeegoimframework.so -lXfixes

SOURCES += \
           main.cpp \
           mpassthruwindow.cpp \

HEADERS += \
           mpassthruwindow.h \

QT += opengl core gui
CONFIG += debug meegotouch

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

QMAKE_CLEAN += *.gcno *.gcda

# note: not installing the service file, the input method server
# is started by other means than by the d-bus.
# it is still required in scratchbox and N900
INSTALLS += target \
            servicefiles \


QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.depends += $$TARGET

QMAKE_EXTRA_TARGETS += check 
check.target = check
check.depends += $$TARGET

system(dpkg-architecture -ei386):SOFTWARE="-software -bypass-wm-hint "
message(Argument added to im server: $$SOFTWARE $$TARGETDEVICE)

SERVICE_FILE = minputmethodserver.service.in

servicefilegenerator.output = minputmethodserver.service
servicefilegenerator.input = SERVICE_FILE
servicefilegenerator.commands += sed -e \"s:M_BIN_PATH:$$M_INSTALL_BIN:g\" -e \"s:SOFTWARE:$$SOFTWARE:g\" -e \"s:TARGET:$$TARGETDEVICE:g\" ${QMAKE_FILE_NAME} > ${QMAKE_FILE_OUT}
servicefilegenerator.CONFIG = target_predeps no_link
QMAKE_EXTRA_COMPILERS += servicefilegenerator

servicefiles.path = $$system(pkg-config --variable session_bus_services_dir dbus-1)
servicefiles.files = minputmethodserver.service
