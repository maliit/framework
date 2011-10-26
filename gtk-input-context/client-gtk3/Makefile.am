# Build a immodule for GTK3, using the same source code as for GTK2
INCLUDES =	-I$(top_builddir) \
		-I$(top_srcdir) \
		-I$(top_srcdir)/src \
		-I$(top_srcdir)/client-gtk
			
noinst_HEADERS =

moduledir = @GTK3_IM_MODULEDIR@

module_LTLIBRARIES = im-maliit.la

im_maliit_la_SOURCES = \
		$(top_srcdir)/client-gtk/gtk-imcontext-plugin.c \
		$(top_srcdir)/client-gtk/client-imcontext-gtk.c \
		$(top_srcdir)/client-gtk/client-imcontext-gtk.h \
		$(top_srcdir)/client-gtk/qt-gtk-translate.cpp \
		$(top_srcdir)/client-gtk/qt-gtk-translate.h

im_maliit_la_CFLAGS  = @MT_IM_BRIDGES_CFLAGS@ @MT_IM_GTK3_CFLAGS@ -Wall -Werror
im_maliit_la_CXXFLAGS  = @MT_IM_BRIDGES_CFLAGS@ @MT_IM_GTK3_CFLAGS@ @QT_CFLAGS@ -Wall -Werror

im_maliit_la_LDFLAGS = -rpath $(moduledir) \
		     -avoid-version -no-undefined \
		     -module \
		     @MT_IM_BRIDGES_LIBS@ @MT_IM_GTK3_LIBS@ @QT_LIBS@

im_maliit_la_LIBADD = $(top_builddir)/src/libmaliitimcommon.la

MAINTAINERCLEANFILES 	= Makefile.in
