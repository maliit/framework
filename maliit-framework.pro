include(./config.pri)

!isEmpty(HELP) {
    # Output help
    help_string = \
        Important build options: \
        \\n\\t M_IM_PREFIX : Install prefix (default: /usr) \
        \\n\\t M_IM_INSTALL_{BIN,LIBS,HEADERS,SCHEMAS,DOCS} : Install prefix for specific types of files \
        \\n\\t M_IM_DEFAULT_PLUGIN : Default onscreen (virtual) keyboard plugin \
        \\n\\t M_IM_DEFAULT_HW_PLUGIN : Default hardware keyboard plugin \
        \\n\\t M_IM_DEFAULT_SUBVIEW : Default onscreen (software) subview name \
        \\n\\t MALIIT_SERVER_ARGUMENTS : Arguments to use for starting maliit-server by D-Bus activation \
        \\nRecognised CONFIG flags: \
        \\n\\t enable-legacy : Build in legacy mode (for meego-im ABI/API compatability) \
        \\n\\t enable-contextkit : Build contextkit support (for monitoring hardware keyboard status) \
        \\n\\t disable-dbus-activation : Do not use dbus activation support for maliit-server \
        \\n\\t disable-gconf : Disable GConf settings backend (falls back to QSettings) \
        \\n\\t notests : Do not build tests \
        \\n\\t nosdk : Do not build Maliit SDK \
        \\n\\t nodoc : Do not build documentation (also disables SDK) \
        \\n\\t only-libmaliit : Only build libmaliit \
        \\n\\t external-libmaliit : Use external libmaliit (do not build libmaliit) \
        \\n\\t disable-gtk-cache-update : Do not update GTK2/3 input method caches (used for packaging) \
        \\n\\t enforce-install-prefix : Always use M_IM_PREFIX instead of prefix reported by GTK+, Qt, DBus etc. (used for testing) \
        \\nInfluential environment variables: \
        \\n\\t GCONF_CONFIG_SOURCE : Specify custom gconf source2 \
        \\n\\t GCONF_DISABLE_MAKEFILE_SCHEMA_INSTALL : Do not register gconf schemas (used for packaging) \
        \\n\\t PKG_CONFIG_PATH : Override standard directories to look for pkg-config information \
        \\nExamples: \
        \\n\\t qmake \
        \\n\\t qmake M_IM_PREFIX=/usr M_IM_INSTALL_LIBS=/usr/lib64 CONFIG+=enable-legacy CONFIG+=notests \
        \\n\\t qmake M_IM_PREFIX=/usr M_IM_DEFAULT_PLUGIN=libmykeyboard.so M_IM_DEFAULT_SUBVIEW=en.xml

    !build_pass:system(echo -e \"$$help_string\")
} else {
    config_string = Tip: Run qmake HELP=1 for a list of all supported build options

    !build_pass:system(echo -e \"$$config_string\")
}

CONFIG += ordered
TEMPLATE = subdirs

!only-libmaliit {
    SUBDIRS = common connection src passthroughserver
    TARGET = meego-im-uiserver
}

external-libmaliit {
    only-libmaliit:error("CONFIG options only-libmaliit and external-libmaliit does not make sense together!")
    !system(pkg-config --exists maliit-1.0):error("Could not find maliit-1.0")
} else {
    SUBDIRS += maliit
}

!only-libmaliit {
    SUBDIRS += gtk-input-context examples

    !contains(QT_MAJOR_VERSION, 5) {
        # Qt 5 has a new platform input plugin system which already contains
        # support for Maliit.
        SUBDIRS += input-context

        # Requires QtQuick1 add-on, which might not be present
        # and we should use QML 2 on Qt 5 anyways
        SUBDIRS += maliit-plugins-quick
    }

    !nodoc {
        SUBDIRS += doc
    }

    !nosdk:!nodoc {
        SUBDIRS += sdk
    }

    !notests {
        SUBDIRS += tests
    }
}

contains(SUBDIRS, input-context) {
    !contains(QT_CONFIG, glib) {
        error(Qt4 input context requires Qt Glib support)
    }
}

!system(pkg-config --exists dbus-glib-1 dbus-1):error("Could not find dbus-glib-1 dbus-1")

!disable-gconf {
    !system(pkg-config --exists gconf-2.0):error("Could not find gconf-2.0")
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
QMAKE_EXTRA_TARGETS += dist
dist.target = dist
dist.commands += git archive HEAD --prefix=$$DIST_NAME/ | bzip2 > $$TARBALL_PATH;
dist.commands += md5sum $$TARBALL_PATH | cut -d \' \' -f 1 > $$DIST_PATH\\.md5

OTHER_FILES += NEWS README
