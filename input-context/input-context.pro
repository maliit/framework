TEMPLATE = subdirs
SUBDIRS = maliit-input-context

CONFIG += direct-connection

!disable-dbus {
    SUBDIRS += input-context-plugin

    input-context-plugin.depends = maliit-input-context
}

direct-connection {
    SUBDIRS += direct-input-context-plugin

    direct-input-context-plugin.depends = maliit-input-context
}
