CONFIG += ordered

TEMPLATE = subdirs

SUBDIRS = \
          ut_duiinputcontextplugin \
          ut_duiinputcontext \
          ut_duiinputcontextadaptor \
          dummyimplugin \
          dummyimplugin2 \
          dummyimplugin3 \
          dummyplugin \
          ut_duiimpluginmanager \
          ut_duiinputcontextdbusconnection \
          ut_passthroughserver \
          ft_duiimpluginmanager \

target.commands += $$system(touch tests.xml)
target.path = /usr/share/dui-im-framework-tests
target.files += qtestlib2junitxml.xsl runtests.sh tests.xml
INSTALLS += target

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.CONFIG = recursive

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive

