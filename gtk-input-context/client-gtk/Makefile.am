INCLUDES =	-I$(top_builddir) \
		-I$(top_srcdir) \
		-I$(top_srcdir)/src
			
noinst_HEADERS =

moduledir = @GTK_IM_MODULEDIR@

module_LTLIBRARIES = im-dui.la


im_dui_la_SOURCES = \
		gtk-imcontext-plugin.c \
		dui-imcontext.c \
		dui-imcontext.h \
		qt-gtk-translate.cpp \
		qt-gtk-translate.h

im_dui_la_CFLAGS  = @DUI_IM_CLIENT_CFLAGS@ @DUI_IM_GTK_CFLAGS@ -Wall -Werror
im_dui_la_CXXFLAGS  = @DUI_IM_CLIENT_CFLAGS@ @DUI_IM_GTK_CFLAGS@ @QT_CFLAGS@ -Wall -Werror

im_dui_la_LDFLAGS = -rpath $(moduledir) \
		     -avoid-version -no-undefined \
		     -module \
		     @DUI_IM_CLIENT_LIBS@ @DUI_IM_GTK_LIBS@ @QT_LIBS@

im_dui_la_LIBADD = $(top_builddir)/src/libimduicommon.la

MAINTAINERCLEANFILES 	= Makefile.in
