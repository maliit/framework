#!/bin/bash

RVAL=0

#
# Stolen from Jani Mikkonen's rt.sh for libdui tests
#
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

#
# Try to figure out, if the script is installed in the system or if we
# are still in the sources tree.
#
TESTCONTEXT="system"
if [ -d `dirname $0`/unit_test -o -d `dirname $0`/../unit_test ]; then
	TESTCONTEXT="local"
fi

#
# Depending on whether this is an automated testrun or a manual call
# from the source tree, LD_LIBRARY_PATH and TESTBASE are set.
#
case "$TESTCONTEXT" in
	local)
		# user executes runtests.sh from source tree
		TMPDIR=.
        #set lib path and plugin path for local running
        export LD_LIBRARY_PATH=../src/:../input-context/
        export TESTPLUGIN_PATH=./plugins/
        export TESTING_IN_SANDBOX=1
		if [ -d unit_test ]; then
			TESTBASE=unit_test
		else
			TESTBASE=.
		fi
		OUTSTYLE="plain"
	;;
	*)
		# tests are run automated on TATAM infrastructure
		TMPDIR=/tmp/dui-im-framework-tests
		TESTBASE=/usr/share/dui-im-framework-tests/
		rm -fr $TMPDIR && mkdir -p $TMPDIR
		OUTSTYLE="xml"
	;;
esac

#
# Run tests and transform output if necessary.
#
for TESTDIR in $TESTBASE/{ut,ft}_*; do
	TESTNAME=`basename $TESTDIR`
	TESTLOG=$TMPDIR/${TESTNAME}.xml
	JUNITLOG=$TMPDIR/${TESTNAME}.log.xml
	if [ -d $TESTDIR ]; then
		if [ -f $TESTDIR/$TESTNAME ]; then
			if [ $OUTSTYLE = "xml" ]; then
				$TESTDIR/$TESTNAME -xml -o $TESTLOG
			else
				$TESTDIR/$TESTNAME
			fi
			EXITCODE=$?
			if [ $EXITCODE = 0 ]; then
				if [ $OUTSTYLE = "xml" ]; then
					xsltproc --nonet `dirname $0`/qtestlib2junitxml.xsl $TESTLOG > $JUNITLOG
				fi
			else
				emptylog $JUNITLOG $TESTNAME "unittest crashed or exited with a fatal error."
			fi
		else
			emptylog $JUNITLOG $TESTNAME "unittest binary not found."
		fi
	fi
done

exit $RVAL

