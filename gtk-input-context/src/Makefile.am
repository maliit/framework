INCLUDES		= -I$(top_builddir) \
	    		  -I$(top_srcdir)

noinst_LTLIBRARIES = libimduicommon.la

dui-im-proxy-glue.h: dui-im-client.xml
	$(DBUS_BINDING_TOOL) --prefix=dui_im_proxy --mode=glib-client --output=$@ $<

dui-imcontext-dbus-glue.h: dui-imcontext-dbus.xml
	$(DBUS_BINDING_TOOL) --prefix=dui_imcontext_dbus --mode=glib-server --output=$@ $<

BUILT_SOURCES = dui-im-proxy-glue.h \
		dui-imcontext-dbus-glue.h

libimduicommon_la_SOURCES = debug.h \
			dui-imcontext-dbus.c \
			dui-imcontext-dbus.h \
			dui-im-proxy.c \
			dui-im-proxy.h \
			qt-keysym-map.h \
			qt-keysym-map.cpp \
			$(BUILT_SOURCES)


libimduicommon_la_CFLAGS  = @DUI_IM_CLIENT_CFLAGS@ -Wall -Werror
libimduicommon_la_CXXFLAGS  = @DUI_IM_CLIENT_CFLAGS@ @QT_CFLAGS@ -Wall -Werror

libimduicommon_la_LDFLAGS = \
		     -avoid-version -no-undefined \
		     @DUI_IM_CLIENT_LIBS@ @QT_LIBS@

EXTRA_DIST = dui-im-client.xml \
		dui-imcontext-dbus.xml

CLEANFILES = $(BUILT_SOURCES)
MAINTAINERCLEANFILES 	= Makefile.in
