TEMPLATE = subdirs
SUBDIRS = embedded

!disable-dbus {
    SUBDIRS += \
        plainqt \
        twofields \
        widgetproperties \
        allinputmethods \
        server-embedded \
        settings \
        settings-python3 \

}

system(pkg-config gtk+-2.0):!disable-dbus {
    !nogtk {
        SUBDIRS += gtk2 gtk2-overrides
    }
}

system(pkg-config gtk+-3.0):!disable-dbus {
    !nogtk {
        SUBDIRS += gtk3 gtk3-overrides gtk3-python
    }
}
