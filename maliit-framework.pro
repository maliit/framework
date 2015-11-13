include(./config.pri)

!isEmpty(HELP) {
    # Output help
    help_string = \
        Important build options: \
        \\n\\t PREFIX : Install prefix (default: /usr) \
        \\n\\t {BIN,LIB,INCLUDE,DOC}DIR : Install prefix for specific types of files \
        \\n\\t MALIIT_DEFAULT_PLUGIN : Default onscreen (virtual) keyboard plugin \
        \\n\\t MALIIT_DEFAULT_HW_PLUGIN : Default hardware keyboard plugin \
        \\n\\t MALIIT_SERVER_ARGUMENTS : Arguments to use for starting maliit-server by D-Bus activation \
        \\nRecognised CONFIG flags: \
        \\n\\t nohwkeyboard : Disable the support for the hardware keyboard \
        \\n\\t enable-contextkit : Build contextkit support (for monitoring hardware keyboard status) \
        \\n\\t enable-dbus-activation : Enable dbus activation support for maliit-server \
        \\n\\t notests : Do not build tests \
        \\n\\t nodoc : Do not build documentation\
        \\n\\t local-install : Install everything underneath PREFIX, nothing to system directories reported by GTK+, Qt, DBus etc. \
        \\n\\t glib : Compile GDBus bindings \
        \\n\\t wayland : Compile with support for wayland \
        \\n\\t qt5-inputcontext : Compile with Qt5 input context, replaces the one currently provided by Qt \
        \\n\\t noxcb : Compile without xcb support \
        \\nInfluential environment variables: \
        \\n\\t PKG_CONFIG_PATH : Override standard directories to look for pkg-config information \
        \\nExamples: \
        \\n\\t qmake \
        \\n\\t qmake PREFIX=/usr LIBDIR=/usr/lib64 CONFIG+=notests \
        \\n\\t qmake PREFIX=/usr MALIIT_DEFAULT_PLUGIN=libmykeyboard.so

    !build_pass:system(echo -e \"$$help_string\")
} else {
    config_string = Tip: Run qmake HELP=1 for a list of all supported build options

    !build_pass:system(echo -e \"$$config_string\")
}

CONFIG += ordered
TEMPLATE = subdirs

contains(QT_MAJOR_VERSION, 4) {
    error("Qt 5 is required. For the Qt 4 input context see maliit-inputcontext-qt4. For a Qt 4 Maliit please use the 0.81 or 0.94-qt4 branches/release series instead")
}

SUBDIRS = common dbus_interfaces

wayland {
    SUBDIRS += weston-protocols
}

SUBDIRS += connection

glib {
    SUBDIRS += maliit-glib
}

SUBDIRS += src passthroughserver examples

qt5-inputcontext {
    SUBDIRS += input-context
}

!nodoc {
    SUBDIRS += doc
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

DIST_NAME = $$MALIIT_PACKAGENAME-$$MALIIT_VERSION
DIST_PATH = $$OUT_PWD/$$DIST_NAME
TARBALL_SUFFIX = .tar.bz2
TARBALL_PATH = $$DIST_PATH$$TARBALL_SUFFIX

# The 'make dist' target
# Creates a tarball
QMAKE_EXTRA_TARGETS += release
release.target = release
release.commands += git archive HEAD --prefix=$$DIST_NAME/ | bzip2 > $$TARBALL_PATH;
release.commands += md5sum $$TARBALL_PATH | cut -d \' \' -f 1 > $$DIST_PATH\\.md5

OTHER_FILES += NEWS README INSTALL.local
