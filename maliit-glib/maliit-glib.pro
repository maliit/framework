include(../config.pri)

VERSION = $$MALIIT_ABI_VERSION
TEMPLATE = lib
TARGET = $${MALIIT_GLIB_LIB}

CONFIG += link_pkgconfig
PKGCONFIG += gobject-2.0

INCLUDEPATH += ..

CONFIG -= qt

HEADERSINSTALL += \
    maliitattributeextension.h \

HEADERS += \
    $$HEADERSINSTALL \
    maliitattributeextensionregistry.h \
    maliitattributeextensionprivate.h \
    maliitmarshallers.h

SOURCES += \
    maliitattributeextension.c \
    maliitattributeextensionregistry.c

target.path += $$M_IM_INSTALL_LIBS

headers.path += $$M_IM_INSTALL_HEADERS/$$MALIIT_HEADER/maliit-glib
headers.files += $$HEADERSINSTALL

outputFiles(maliit-glib-$${MALIIT_INTERFACE_VERSION}.pc)

OTHER_FILES += \
    maliit-glib-$${MALIIT_INTERFACE_VERSION}.pc.in

install_pkgconfig.path = $${M_IM_INSTALL_LIBS}/pkgconfig
install_pkgconfig.files = $$OUT_PWD/maliit-glib-$${MALIIT_INTERFACE_VERSION}.pc

INSTALLS += \
    target \
    headers \
    install_pkgconfig \

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

QMAKE_CLEAN += \
    $$OBJECTS_DIR/*.gcno \
    $$OBJECTS_DIR/*.gcda \
    maliitmarshallers.h \
    maliitmarshallers.c

OTHER_FILES += \
    libmaliit-glib.pri

# generate marshallers
GLIB_GENMARSHAL_LIST += maliitmarshallers.list

OTHER_FILES += maliitmarshallers.list

glib_genmarshal_header.name = glib-genmarshal header ${QMAKE_FILE_IN}
glib_genmarshal_header.commands = glib-genmarshal --prefix=maliit_marshal --header --g-fatal-warnings ${QMAKE_FILE_IN} > ${QMAKE_FILE_OUT}
glib_genmarshal_header.output = ${QMAKE_FILE_IN_BASE}.h
glib_genmarshal_header.variable_out = HEADERS
glib_genmarshal_header.input = GLIB_GENMARSHAL_LIST

glib_genmarshal_source.name = glib-genmarshal source ${QMAKE_FILE_IN}
glib_genmarshal_source.commands = glib-genmarshal --prefix=maliit_marshal --body --g-fatal-warnings ${QMAKE_FILE_IN} > ${QMAKE_FILE_OUT}
glib_genmarshal_source.output = ${QMAKE_FILE_IN_BASE}.c
glib_genmarshal_source.variable_out = SOURCES
glib_genmarshal_source.input = GLIB_GENMARSHAL_LIST

QMAKE_EXTRA_COMPILERS += glib_genmarshal_header glib_genmarshal_source
