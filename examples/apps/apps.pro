TEMPLATE = subdirs
SUBDIRS = embedded

!disable-dbus {
    SUBDIRS += plainqt twofields widgetproperties allinputmethods server-embedded
}

system(pkg-config gtk+-2.0):!disable-dbus {
    SUBDIRS += gtk2
}

system(pkg-config gtk+-3.0):!disable-dbus {
    SUBDIRS += gtk3
}
