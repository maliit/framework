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
    maliitattributeextensionregistry.c \
    maliitmarshallers.c

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

QMAKE_EXTRA_TARGETS += maliit_marshallers_h maliit_marshallers_c
mallit_marshallers_h.target = $$OUT_PWD/maliitmarshallers.h
maliit_marshallers_h.output = $$OUT_PWD/maliitmarshallers.h
maliit_marshallers_h.depends = $$IN_PWD/maliitmarshallers.list
maliit_marshallers_h.commands = \
    glib-genmarshal --prefix=maliit_marshal --header --internal --g-fatal-warnings --stdinc $$IN_PWD/maliitmarshallers.list >$$OUT_PWD/maliitmarshallers.h

mallit_marshallers_c.target = $$OUT_PWD/maliitmarshallers.c
maliit_marshallers_c.output = $$OUT_PWD/maliitmarshallers.c
maliit_marshallers_c.depends = $$IN_PWD/maliitmarshallers.list
maliit_marshallers_c.commands = \
    glib-genmarshal --prefix=maliit_marshal --body --internal --g-fatal-warnings --stdinc $$IN_PWD/maliitmarshallers.list >$$OUT_PWD/maliitmarshallers.c

# Use to work around the fact that qmake looks up the target for the generated header wrong
QMAKE_EXTRA_TARGETS += fake_maliit_marshallers_h fake_maliit_marshallers_c

fake_maliit_marshallers_h.target = maliitmarshallers.h
fake_maliit_marshallers_h.depends = maliit_marshallers_h

fake_maliit_marshallers_c.target = maliitmarshallers.c
fake_maliit_marshallers_c.depends = maliit_marshallers_c
