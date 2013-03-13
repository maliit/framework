TEMPLATE = subdirs

!contains(QT_MAJOR_VERSION, 5) {
    SUBDIRS = embedded
} else {
    SUBDIRS =
}

!disable-dbus {
    SUBDIRS += plainqt
    !contains(QT_MAJOR_VERSION, 5) {
        SUBDIRS += \
            twofields \
            widgetproperties \
            allinputmethods \
            server-embedded \
            settings \
            settings-python3 \
    }
}
