include(../config.pri)

DOXYGEN_BIN=doxygen

QMAKE_EXTRA_TARGETS += doc
target = doc

outputFiles(mdoxy.cfg)

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
    htmldocs.files = $${OUT_PWD}/doc/html/*
    htmldocs.path = $$M_IM_INSTALL_DOCS/$$MALIIT_PACKAGENAME
    htmldocs.CONFIG += no_check_exist
    INSTALLS += htmldocs
}

doc.depends = FORCE
