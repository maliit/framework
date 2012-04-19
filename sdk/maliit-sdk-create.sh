#!/bin/bash

### This script creates the Maliit SDK for distribution

EXPECTED_ARGS=4
ERROR_BADARGS=65
QMAKE_EXEC=qmake

# Find proper qmake binary, cmake style ...
for binary in qmake qmake4 qmake-qt4 qmake-mac
do
  which ${binary}
  if [ $? -eq "0" ]
  then
    QMAKE_EXEC=${binary}
    break
  fi
done

if [ $# -ne $EXPECTED_ARGS ]
then
  echo "Usage: `basename $0` SRC_DIR OUT_DIR VERSION MODE"
  exit $ERROR_BADARGS
fi

IN=$1
OUT=$2
VERSION=$3
MODE=$4

case $MODE in
    normal )
        BUILD_TYPE=skeleton
        ;;
    * )
        echo "Wrong MODE argument: \"$MODE\". Has to be \"normal\""
        exit $ERROR_BADARGS
        ;;
esac


EXAMPLES_PATH=$IN/examples
DOC_PATH=$IN/doc/html

SDK_DIR=maliit-sdk
SDK_OUT_PATH=$OUT/$SDK_DIR
SDK_OUT_FILE_PATH=$OUT/maliit-sdk-$VERSION.tar.gz

if [ -d $SDK_OUT_PATH ] ; then rm -r $SDK_OUT_PATH ; fi
mkdir -p $SDK_OUT_PATH/examples

# Extract examples
cp -r $EXAMPLES_PATH $SDK_OUT_PATH/
cd $SDK_OUT_PATH/examples
sed -i -e s/"^BUILD_TYPE.*=.*"/"BUILD_TYPE = $BUILD_TYPE"/ `grep -r --files-with-matches BUILD_TYPE ./ | tr "\n" " "`
# Clean examples if built in-tree
if [ -f Makefile ]
then
    ${QMAKE_EXEC} -r || exit 1
    make distclean || exit 1
    find ./ -type d \( -name ".obj" -o -name ".moc" \) -print0 | xargs -0 /bin/rmdir
fi

rm README
cd -

# Pack
cd $OUT
tar -acf $SDK_OUT_FILE_PATH $SDK_DIR
cd -
