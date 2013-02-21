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
        \\n\\t enable-contextkit : Build contextkit support (for monitoring hardware keyboard status) \
        \\n\\t enable-dbus-activation : Enable dbus activation support for maliit-server \
        \\n\\t disable-dbus : Disable dbus communication backend \
        \\n\\t enable-qdbus : Enable QtDBus implementation for the DBus communication backend \
        \\n\\t notests : Do not build tests \
        \\n\\t noqml : Do not build Quick QML Plugins Interface\
        \\n\\t nogtk : Do not build GTK+ input method module, nor the glib-based application support libraries\
        \\n\\t nodoc : Do not build documentation\
        \\n\\t disable-gtk-cache-update : Do not update GTK2/3 input method caches (used for packaging) \
        \\n\\t local-install : Install everything underneath PREFIX, nothing to system directories reported by GTK+, Qt, DBus etc. \
        \\n\\t disable-background-translucency : Do not set translucent background hint on surfaces (workaround for non-compositing WMs) \
        \\n\\t wayland : Compile with support for wayland \
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

SUBDIRS = common

!disable-dbus {
    SUBDIRS += dbus_interfaces
}

contains(QT_MAJOR_VERSION, 4) {
    message("Qt 4 is just supported to compile the Qt 4 input context. \
             For a Qt 4 Maliit please use the 0.81 or 0.94-qt4 branches/release series instead")

    # https://bugs.maliit.org/show_bug.cgi?id=47#c4
    SUBDIRS += connection maliit maliit-settings input-context
} else {
    SUBDIRS += connection src

    !disable-dbus {
        SUBDIRS += passthroughserver connection-glib maliit-glib
        !nodoc {
            SUBDIRS += maliit-glib/maliit-glib-docs.pro
        }
        SUBDIRS += gtk-input-context
    }

    nogtk {
        SUBDIRS -= connection-glib maliit-glib gtk-input-context
    }

    # Requires QtQuick1 add-on, which might not be present
    # and we should use QML 2 on Qt 5 anyways
    !noqml {
        SUBDIRS += maliit-plugins-quick
    }

    SUBDIRS += examples

    !nodoc {
        SUBDIRS += doc
    }

    !notests {
        SUBDIRS += tests
    }
}

!disable-dbus {
    !system(pkg-config --exists dbus-glib-1 dbus-1):error("Could not find dbus-glib-1 dbus-1")
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

OTHER_FILES += NEWS README INSTALL.local
