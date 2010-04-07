INCLUDES		= -I$(top_builddir) \
	    		  -I$(top_srcdir)

noinst_HEADERS =

moduledir = @GTK_IM_MODULEDIR@

module_LTLIBRARIES = im-dui.la

dui-im-proxy-glue.h: dui-im-client.xml
	$(DBUS_BINDING_TOOL) --prefix=dui_im_proxy --mode=glib-client --output=$@ $<

dui-imcontext-dbus-glue.h: dui-imcontext-dbus.xml
	$(DBUS_BINDING_TOOL) --prefix=dui_imcontext_dbus --mode=glib-server --output=$@ $<

BUILT_SOURCES = dui-im-proxy-glue.h \
		dui-imcontext-dbus-glue.h

im_dui_la_SOURCES = debug.h \
			gtk-imcontext-plugin.c \
			dui-imcontext.c \
			dui-imcontext.h \
			dui-imcontext-dbus.c \
			dui-imcontext-dbus.h \
			dui-im-proxy.c \
			dui-im-proxy.h \
			qt-gtk-translate.cpp \
			qt-gtk-translate.h \
			qt-keysym-map.h \
			qt-keysym-map.cpp \
			$(BUILT_SOURCES)


im_dui_la_CFLAGS  = @DUI_IM_GTKIC_CFLAGS@ -Wall -Werror
im_dui_la_CXXFLAGS  = @DUI_IM_GTKIC_CFLAGS@ @QT_CFLAGS@ -Wall -Werror

im_dui_la_LDFLAGS = -rpath $(moduledir) \
		     -avoid-version -no-undefined \
		     -module \
		     @DUI_IM_GTKIC_LIBS@ @QT_LIBS@

EXTRA_DIST = dui-im-client.xml \
		dui-imcontext-dbus.xml

CLEANFILES = $(BUILT_SOURCES)
MAINTAINERCLEANFILES 	= Makefile.in
