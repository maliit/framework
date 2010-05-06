INCLUDES		= -I$(top_builddir) \
	    		  -I$(top_srcdir)

noinst_LTLIBRARIES = libmeegoimcommon.la

meego-im-proxy-glue.h: meego-im-client.xml
	$(DBUS_BINDING_TOOL) --prefix=meego_im_proxy --mode=glib-client --output=$@ $<

meego-imcontext-dbus-glue.h: meego-imcontext-dbus.xml
	$(DBUS_BINDING_TOOL) --prefix=meego_imcontext_dbus --mode=glib-server --output=$@ $<

BUILT_SOURCES = meego-im-proxy-glue.h \
		meego-imcontext-dbus-glue.h

libmeegoimcommon_la_SOURCES = debug.h \
			meego-imcontext-dbus.c \
			meego-imcontext-dbus.h \
			meego-im-proxy.c \
			meego-im-proxy.h \
			qt-keysym-map.h \
			qt-keysym-map.cpp \
			$(BUILT_SOURCES)


libmeegoimcommon_la_CFLAGS  = @MT_IM_BRIDGES_CFLAGS@ -Wall -Werror
libmeegoimcommon_la_CXXFLAGS  = @MT_IM_BRIDGES_CFLAGS@ @QT_CFLAGS@ -Wall -Werror

libmeegoimcommon_la_LDFLAGS = \
		     -avoid-version -no-undefined \
		     @MT_IM_BRIDGES_LIBS@ @QT_LIBS@

EXTRA_DIST = meego-im-client.xml \
		meego-imcontext-dbus.xml

CLEANFILES = $(BUILT_SOURCES)
MAINTAINERCLEANFILES 	= Makefile.in
