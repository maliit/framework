CONFIG += ordered
TARGET = m-im-uiserver
TEMPLATE = subdirs
SUBDIRS = src passthroughserver input-context unit_test

isEqual( IN_PWD, $${OUT_PWD} ) {
    IS_OUT_OF_SOURCE = 0
} else {
    IS_OUT_OF_SOURCE = 1
}

include (doc/doc.pri)

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.CONFIG = recursive

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive

GCONF_DATA = m-im-framework.schemas
gconf_data.path = /usr/share/gconf/schemas
gconf_data.files = $$GCONF_DATA

INSTALLS += gconf_data
