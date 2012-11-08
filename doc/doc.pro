include(../config.pri)

DOXYGEN_BIN=doxygen

TEMPLATE = lib
CONFIG += plugin
TARGET = dummy

outputFiles(mdoxy.cfg)

DOXYGEN = .

HTML_BUILD_DIR = $${OUT_PWD}/html/
# qmake creates wrong install rules for directories
# that do not exist at qmake time, so we hack it here
system(mkdir -p $$HTML_BUILD_DIR)

doc.name = doc
doc.CONFIG += target_predeps no_link
doc.output = $$HTML_BUILD_DIR/index.html
doc.clean_commands = rm -rf $$HTML_BUILD_DIR/*
doc.clean = doxygen.log doxygen.log.xml
doc.input = DOXYGEN

isEmpty(DOXYGEN_BIN) {
    doc.commands = @echo "Unable to detect doxygen in PATH"
} else {
    # Build docs
    doc.commands += $${DOXYGEN_BIN} $${OUT_PWD}/mdoxy.cfg;
    doc.commands += cp $${PWD}/src/images/* $$HTML_BUILD_DIR ;
    doc.commands += cp $${PWD}/src/*.html $$HTML_BUILD_DIR ;
    doc.commands += $${PWD}/xmlize.pl;

    # Install rules
    htmldocs.files = $$HTML_BUILD_DIR
    htmldocs.path = $$DOCDIR/$$MALIIT_PACKAGENAME
    htmldocs.CONFIG += no_check_exist directory
    INSTALLS += htmldocs
}

QMAKE_EXTRA_COMPILERS += doc
