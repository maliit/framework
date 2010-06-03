include ($$[QT_INSTALL_DATA]/mkspecs/dcpconfig.pri)
# desktop files:
QMAKE_EXTRA_TARGET += desktop
desktop.files += *.desktop
desktop.path = $$DCP_DESKTOP_DIR
INSTALLS += desktop
