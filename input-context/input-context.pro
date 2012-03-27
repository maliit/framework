TEMPLATE = subdirs
SUBDIRS = maliit-input-context input-context-plugin

CONFIG += direct-connection

input-context-plugin.depends = maliit-input-context

direct-connection {
    SUBDIRS += direct-input-context-plugin

    direct-input-context-plugin.depends = maliit-input-context
}
