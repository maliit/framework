TOP_DIR = ..

include(../config.pri)

TEMPLATE=lib
TARGET=dummy

HEADERS += \
    maliitattributeextension.h \
    maliitinputmethod.h \


# The resulting html docs go into ./maliit, and the temporary build files to ./reference/
gtk_doc.name = gtk-doc
gtk_doc.CONFIG += target_predeps no_link combine
gtk_doc.output = $${OUT_PWD}/maliit/index.html
gtk_doc.clean_commands = rm -rf $${OUT_PWD}/maliit $${OUT_PWD}/reference
gtk_doc.input = HEADERS
gtk_doc.commands += mkdir -p reference &&
gtk_doc.commands += cp $$IN_PWD/maliit-sections.txt $$IN_PWD/maliit-docs.xml $$OUT_PWD/reference &&
gtk_doc.commands += cd reference &&
gtk_doc.commands += gtkdoc-scan --module=maliit --source-dir=$${IN_PWD} --rebuild-types  &&
gtk_doc.commands += LD_LIBRARY_PATH=\"$${OUT_PWD}\" CFLAGS=\"$$system(pkg-config --cflags gio-2.0)\" LDFLAGS=\"-L$${OUT_PWD} -l$${MALIIT_GLIB_LIB} $$system(pkg-config --libs gio-2.0)\"
gtk_doc.commands += gtkdoc-scangobj --module=maliit &&
gtk_doc.commands += gtkdoc-mkdb --module=maliit --source-dir=$${IN_PWD} --output-format=xml && cd .. &&
gtk_doc.commands += mkdir -p maliit && cd maliit && gtkdoc-mkhtml maliit ../reference/maliit-docs.xml && cd .. &&
gtk_doc.commands += cd reference && gtkdoc-fixxref --module=maliit --module-dir=../maliit && cd ..

docs.files = $${OUT_PWD}/maliit
docs.path = $$DATADIR/gtk-doc/html
docs.CONFIG += no_check_exist directory
INSTALLS += docs

QMAKE_EXTRA_COMPILERS += gtk_doc

OTHER_FILES += maliit-sections.txt maliit-docs.xml
