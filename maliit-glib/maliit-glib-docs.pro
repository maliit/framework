TOP_DIR = ..

include(../config.pri)

TEMPLATE=lib
TARGET=dummy

GOBJECTFILES += \
    maliitattributeextension.c \
    maliitattributeextension.h \
    maliitinputmethod.c \
    maliitinputmethod.h \
    maliitsettingsmanager.c \
    maliitsettingsmanager.h \
    maliitsettingsentry.c \
    maliitsettingsentry.h \
    maliitpluginsettings.c \
    maliitpluginsettings.h \
    maliitsettingdata.c \
    maliitsettingdata.h \


# The resulting html docs go into ./maliit, and the temporary build files to ./reference/
gtk_doc.name = gtk-doc
gtk_doc.CONFIG += target_predeps no_link combine
gtk_doc.output = $${OUT_PWD}/maliit/index.html
gtk_doc.clean_commands = rm -rf $${OUT_PWD}/maliit $${OUT_PWD}/reference
gtk_doc.input = GOBJECTFILES
gtk_doc.commands += mkdir -p reference &&
gtk_doc.commands += cp $$IN_PWD/maliit-sections.txt $$IN_PWD/maliit-docs.xml $$OUT_PWD/reference &&
gtk_doc.commands += cd reference &&
gtk_doc.commands += gtkdoc-scan --module=maliit --source-dir=$${IN_PWD} --rebuild-types  &&
gtk_doc.commands += LD_LIBRARY_PATH=\"$${OUT_PWD}\" CFLAGS=\"$$system(pkg-config --cflags gio-2.0)\" LDFLAGS=\"-L$${OUT_PWD} -l$${MALIIT_GLIB_LIB} $$system(pkg-config --libs gio-2.0)\"
gtk_doc.commands += gtkdoc-scangobj --module=maliit &&
gtk_doc.commands += gtkdoc-mkdb --module=maliit --source-dir=$${IN_PWD} --output-format=xml && cd .. &&
gtk_doc.commands += mkdir -p maliit && cd maliit && gtkdoc-mkhtml maliit ../reference/maliit-docs.xml && cd .. &&
gtk_doc.commands += cd reference && gtkdoc-fixxref --module=maliit --module-dir=../maliit && cd ..

gir_scanner.name = g-ir-scanner
gir_scanner.CONFIG += no_link combine
gir_scanner.output = $${OUT_PWD}/Maliit-1.0.gir
gir_scanner.input = GOBJECTFILES
gir_scanner.commands += g-ir-scanner -n Maliit --no-libtool -L$${OUT_PWD} --library=maliit-glib-1.0 --include=Gio-2.0 --pkg=gio-2.0 --nsversion=1.0 --output=${QMAKE_FILE_OUT} ${QMAKE_FILE_IN}

GIR_FILES = $${OUT_PWD}/Maliit-1.0.gir

gir_compiler.name = g-ir-compiler
gir_compiler.CONFIG += target_predeps no_link no_check_exist
gir_compiler.output = $${OUT_PWD}/Maliit-1.0.typelib
gir_compiler.input = GIR_FILES
gir_compiler.commands += g-ir-compiler -m Maliit --output=${QMAKE_FILE_OUT} ${QMAKE_FILE_IN}

system(pkg-config gtk-doc) {
    docs.files = $${OUT_PWD}/maliit
    docs.path = $$DATADIR/gtk-doc/html
    docs.CONFIG += no_check_exist directory

    INSTALLS += docs

    QMAKE_EXTRA_COMPILERS += gtk_doc
}

system(pkg-config gobject-introspection-1.0) {
    GIR_DIR = $$system(pkg-config --variable=girdir gobject-introspection-1.0)
    TYPELIB_DIR = $$system(pkg-config --variable=typelibdir gobject-introspection-1.0)
    GIR_PREFIX = $$system(pkg-config --variable=prefix gobject-introspection-1.0)
    GIR_DIR = $$replace(GIR_DIR, $$GIR_PREFIX, $$PREFIX)
    TYPELIB_DIR = $$replace(TYPELIB_DIR, $$GIR_PREFIX, $$PREFIX)

    gir.files = $${OUT_PWD}/Maliit-1.0.gir
    gir.path = $$GIR_DIR
    gir.CONFIG += no_check_exist

    typelib.files = $${OUT_PWD}/Maliit-1.0.typelib
    typelib.path = $$TYPELIB_DIR
    typelib.CONFIG += no_check_exist

    INSTALLS += gir typelib

    QMAKE_EXTRA_COMPILERS += gir_scanner gir_compiler
}

OTHER_FILES += maliit-sections.txt maliit-docs.xml
