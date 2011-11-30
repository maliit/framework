include(../config.pri)

DOXYGEN_BIN=doxygen

TEMPLATE = lib
CONFIG += plugin
TARGET = dummy

outputFiles(mdoxy.cfg)

DOXYGEN = .

doc.name = doc
doc.CONFIG += target_predeps no_link
doc.output = html
doc.clean_commands = rm -rf html
doc.clean = doxygen.log doxygen.log.xml
doc.input = DOXYGEN

isEmpty(DOXYGEN_BIN) {
    doc.commands = @echo "Unable to detect doxygen in PATH"
} else {
    # Build docs
    doc.commands += mkdir -p $${OUT_PWD}/html ;
    doc.commands += ( $${DOXYGEN_BIN} $${OUT_PWD}/mdoxy.cfg );
    doc.commands += cp $${IN_PWD}/src/images/* $${OUT_PWD}/html ;
    doc.commands += cp $${IN_PWD}/src/*.html $${OUT_PWD}/html ;
    doc.commands += ( $${IN_PWD}/xmlize.pl );

    # Install rules
    htmldocs.files = $${OUT_PWD}/html/
    htmldocs.path = $$M_IM_INSTALL_DOCS/$$MALIIT_PACKAGENAME
    htmldocs.CONFIG += no_check_exist directory
    INSTALLS += htmldocs
}

QMAKE_EXTRA_COMPILERS += doc
