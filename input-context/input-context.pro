TEMPLATE = subdirs
SUBDIRS = maliit-input-context

!disable-dbus {
    SUBDIRS += input-context-plugin

    input-context-plugin.depends = maliit-input-context
}

OTHER_FILES += \
    libmaliit-inputcontext-qt4.pri
