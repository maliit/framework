INCLUDES		= -I$(top_builddir) \
	    		  -I$(top_srcdir)

noinst_HEADERS =

moduledir = @GTK_IM_MODULEDIR@

module_LTLIBRARIES = im-dui.la

dui-im-proxy-glue.h: dui-im-client.xml
	$(DBUS_BINDING_TOOL) --prefix=dui_im_proxy --mode=glib-client --output=$@ $<

BUILT_SOURCES = dui-im-proxy-glue.h

im_dui_la_SOURCES = gtk-imcontext-plugin.c \
			dui-imcontext.c \
			dui-imcontext.h \
			dui-im-proxy.c \
			dui-im-proxy.h \
			$(BUILT_SOURCES)

im_dui_la_CFLAGS  = @DUI_IM_GTKIC_CFLAGS@ -Wall -Werror

im_dui_la_LDFLAGS = -rpath $(moduledir) \
		     -avoid-version -no-undefined \
		     -module \
		     @DUI_IM_GTKIC_LIBS@

EXTRA_DIST = dui-im-client.xml

CLEANFILES = $(BUITSOURCES)
MAINTAINERCLEANFILES 	= Makefile.in
