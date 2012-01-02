TEMPLATE = subdirs
SUBDIRS = plainqt twofields widgetproperties allinputmethods

system(pkg-config gtk+-2.0) {
    SUBDIRS += gtk2
}

system(pkg-config gtk+-3.0) {
    SUBDIRS += gtk3
}
