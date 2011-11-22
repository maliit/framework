#!/bin/bash
# this is just small wrapper to automate the unit testcases

dbus-uuidgen --ensure

function emptylog() {
    FILENAME=$1
    TESTCASE=$2
    REASON=$3

    cat<<EOF > $FILENAME
<?xml version="1.0"?>
<testsuite name="${TESTCASE}" tests="3" failures="0" errors="1" time="0.0">
  <properties>
    <property name="QtVersion" value="4.4.0"/>
    <property name="QTestVersion" value="4.4.0"/>
  </properties>
  <testcase classname="${TESTCASE}" name="initTestCase" time="0.0"/>
  <testcase classname="${TESTCASE}" name="notAvailable" time="0.0">
    <failure type="Standard" message="${REASON} unittest crashed or exited with fatal error"/>
  </testcase>
  <testcase classname="${TESTCASE}" name="cleanupTestCase" time="0.0"/>
  <system-out>Current testsuite crashes or wasnt in shape to be executed in development environment and this error message is placeholder. Fix your tests!</system-out>
  <system-err/>
</testsuite>
EOF

}

TEST=$1
CURDIR=`pwd`
DIRNAME=`dirname $TEST`
BASENAME=`basename $TEST`
QTESTLOG=./$BASENAME.log
JUNITLOG=./$BASENAME.log.xml

set -x
# cd $DIRNAME
if [ ! -f ./$BASENAME ]; then
    EXIT_CODE=255
else
    TESTING_IN_SANDBOX=1 TESTPLUGIN_PATH=../plugins LD_LIBRARY_PATH=../../src:../../input-context/:../../common:../../maliit-plugins-quick/input-method:../../maliit-plugins-quick/plugin-factory:$LD_LIBRARY_PATH dui-run ./$BASENAME -xml -o ./$BASENAME.log
    xsltproc --nonet ../qtestlib2junitxml.xsl $QTESTLOG > $JUNITLOG
    EXIT_CODE=$?
fi

case "$EXIT_CODE" in
    "0")
        echo success
    ;;
    "255")
        echo error: unittest $BASENAME is not compiled
        emptylog $JUNITLOG $BASENAME "unittest doesnt exist"
    ;;
    *)
        echo error: unittest $BASENAME failed with $EXIT_CODE errors
        emptylog $JUNITLOG $BASENAME "unittest crashed or exited with fatal error or is using QTEST_MAIN()"
    ;;
esac
cd $CURDIR
exit 0
