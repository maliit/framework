include(./config.pri)

CONFIG += ordered
TARGET = meego-im-uiserver
TEMPLATE = subdirs
SUBDIRS = src passthroughserver

nomeegotouch {
    SUBDIRS += maliit
}

SUBDIRS += input-context input-method-quick examples

!nodoc {
    SUBDIRS += doc
}

!nomeegotouch {
    CONFIG  += meegotouch
    SUBDIRS += translations
}

!nomeegotouch:!noduicontrolpanel {
    SUBDIRS += settings-applet
}

!notests {
    SUBDIRS += tests
}

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.CONFIG = recursive

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive

