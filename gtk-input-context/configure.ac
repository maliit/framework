AC_PREREQ(2.61)

# package version number (not shared library version)
# odd micro numbers indicate in-progress development
# even micro numbers indicate released versions
m4_define(meegotouch_inputmethodbridges_version_major, 0)
m4_define(meegotouch_inputmethodbridges_version_minor, 2)
m4_define(meegotouch_inputmethodbridges_version_micro, 2)

m4_define([meegotouch_inputmethodbridges_version],
          [meegotouch_inputmethodbridges_version_major.meegotouch_inputmethodbridges_version_minor.meegotouch_inputmethodbridges_version_micro])
m4_define([meegotouch_inputmethodbridges_api_version],
          [meegotouch_inputmethodbridges_version_major.meegotouch_inputmethodbridges_version_minor])

AC_INIT(meegotouch-inputmethodbridges, meegotouch_inputmethodbridges_version)
AM_INIT_AUTOMAKE(meegotouch-inputmethodbridges, meegotouch_inputmethodbridges_version)
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])

# Check for programs
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_INSTALL

# enable pkg-config
PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES(MT_IM_BRIDGES, glib-2.0
			gthread-2.0
			dbus-glib-1)

AC_SUBST(MT_IM_BRIDGES_CFLAGS)
AC_SUBST(MT_IM_BRIDGES_LIBS)

PKG_CHECK_MODULES(MT_IM_GTK,
			gtk+-2.0 >= 2.14)

AC_SUBST(MT_IM_GTK_CFLAGS)
AC_SUBST(MT_IM_GTK_LIBS)


PKG_CHECK_MODULES(QT, QtCore
			QtGui >= 4.6)
AC_SUBST(QT_CFLAGS)
AC_SUBST(QT_LIBS)

DBUS_BINDING_TOOL="dbus-binding-tool"
AC_SUBST(DBUS_BINDING_TOOL)

gtk_binary_version=`$PKG_CONFIG --variable=gtk_binary_version gtk+-2.0`
GTK_IM_MODULEDIR="$libdir"/gtk-2.0/$gtk_binary_version/immodules
AC_SUBST(GTK_IM_MODULEDIR)

###########################################################
##             Definiation of compile args.              ##
###########################################################

AC_ARG_ENABLE(debug,
              [  --enable-debug            Turn on debugging], ,
              enable_debug=no)

if test "$enable_debug" = "yes"; then
  AC_DEFINE(ENABLE_DEBUG,1,[Define this to enable the debug facility])
  CFLAGS="$CFLAGS -g"
fi

###########################################################


# i18n
#GETTEXT_PACKAGE="meegotouch_inputmethodbridges"
#AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Package name for gettext])
#AC_SUBST(GETTEXT_PACKAGE)
#AS_ALL_LINGUAS
#AM_GLIB_GNU_GETTEXT
#IT_PROG_INTLTOOL([0.40.4])

# Output files
AC_OUTPUT([Makefile
src/Makefile
client-gtk/Makefile
])

AC_MSG_RESULT([
Build options:
  Version                  meegotouch_inputmethodbridges_version
  Install prefix           $prefix
  Enable debug             $enable_debug
])
