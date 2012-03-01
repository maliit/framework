TEMPLATE = subdirs
SUBDIRS = cxx

!contains(QT_MAJOR_VERSION, 5) {
	# Requires QtQuick1 add-on, which might not be present
	# and we should use QML 2 on Qt 5 anyways
	SUBDIRS += qml
}

