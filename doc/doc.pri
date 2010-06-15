DOXYGEN_BIN=doxygen

QMAKE_EXTRA_UNIX_TARGETS += doc
doc.target = doc
isEmpty(DOXYGEN_BIN) {
    doc.commands = @echo "Unable to detect doxygen in PATH"
} else {
  isEqual( IS_OUT_OF_SOURCE, 0 ) {
    # in-source build
    doc.commands = $${DOXYGEN_BIN} $${IN_PWD}/mdoxy.cfg ;
    doc.commands+= mkdir -p doc/html ;
    doc.commands+= cp $${IN_PWD}/src/images/* doc/html ;
    doc.commands+= cp $${IN_PWD}/*.html doc/html ;
    doc.commands+= $${IN_PWD}/xmlize.pl ;

    # Install rules
    htmldocs.files = ./doc/html/*

  } else {
    # out-of-source build
    doc.commands = mkdir -p doc/html/ ;
    doc.commands+= ( cat $${IN_PWD}/mdoxy.cfg.in | \
        perl -pe \"s:\@M_SRC_DIR\@:$${IN_PWD}:\" > $${OUT_PWD}/doc/mdoxy.cfg );

    doc.commands+= ( cd doc ; $${DOXYGEN_BIN} mdoxy.cfg );
    doc.commands+= cp $${IN_PWD}/src/images/* $${OUT_PWD}/doc/html ;
    doc.commands+= ( cd doc ; $${IN_PWD}/xmlize.pl );

    # Install rules
    htmldocs.files = $${OUT_PWD}/doc/html/*
  }

  htmldocs.path = /usr/share/doc/meego-im-framework
  htmldocs.CONFIG += no_check_exist
  INSTALLS += htmldocs
}

doc.depends = FORCE
