TEMPLATE = subdirs
SUBDIRS = cxx

# Needs to be ported to Quick 2 (maybe nemo-keyboard is a better example?)
!contains(QT_MAJOR_VERSION, 5) {
    SUBDIRS += qml
}

