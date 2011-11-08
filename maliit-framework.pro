include(./config.pri)

!isEmpty(HELP) {
    # Output help
    help_string = \
        Important build options: \
        \\n\\t M_IM_PREFIX : Install prefix (default: /usr) \
        \\n\\t M_IM_INSTALL_{BIN,LIBS,HEADERS,SCHEMAS,DOCS} : Install prefix for specific types of files \
        \\n\\t MALIIT_SERVER_ARGUMENTS : Arguments to use for starting maliit-server by D-Bus activation \
        \\nRecognised CONFIG flags: \
        \\n\\t enable-legacy : Build in legacy mode (for meego-im ABI/API compatability) \
        \\n\\t enable-meegotouch : Link input context to libmeegotouch for a MApplicationPage hack \
        \\n\\t enable-contextkit : Build contextkit support (for monitoring hardware keyboard status) \
        \\n\\t notests : Do not build tests \
        \\n\\t nosdk : Do not build Maliit SDK \
        \\n\\t nodoc : Do not build documentation (also disables SDK) \
        \\n\\t only-libmaliit : Only build libmaliit \
        \\n\\t external-libmaliit : Use external libmaliit (do not build libmaliit) \
        \\n\\t disable-gtk-cache-update : Do not update GTK2/3 input method caches \
        \\nInfluential environment variables: \
        \\n\\t GCONF_CONFIG_SOURCE : Specify custom gconf source2 \
        \\nExamples: \
        \\n\\t qmake \
        \\n\\t qmake M_IM_PREFIX=/usr M_IM_INSTALL_LIBS=/usr/lib64 CONFIG+=enable-legacy CONFIG+=notests \

    !build_pass:system(echo -e \"$$help_string\")
} else {
    config_string = Tip: Run qmake HELP=1 for a list of all supported build options

    !build_pass:system(echo -e \"$$config_string\")
}

CONFIG += ordered
TEMPLATE = subdirs

!only-libmaliit {
    SUBDIRS = src passthroughserver
    TARGET = meego-im-uiserver
}

external-libmaliit {
    only-libmaliit:error("CONFIG options only-libmaliit and external-libmaliit does not make sense together!")
    !system(pkg-config --exists maliit-1.0):error("Could not find maliit-1.0")
} else {
    SUBDIRS += maliit
}

!only-libmaliit {
    SUBDIRS += common input-context gtk-input-context maliit-plugins-quick examples

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

!system(pkg-config --exists dbus-glib-1 dbus-1 gconf-2.0):error("Could not find dbus-glib-1 dbus-1 gconf-2.0")

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.CONFIG = recursive

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive

OTHER_FILES += NEWS README
