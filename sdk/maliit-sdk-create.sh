#!/bin/bash

### This script creates the Maliit SDK for distribution

EXPECTED_ARGS=4
ERROR_BADARGS=65

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
    legacy )
        BUILD_TYPE=skeleton-legacy
        ;;
    * )
        echo "Wrong MODE argument: \"$MODE\". Has to be either \"normal\" or \"legacy\""
        exit $ERROR_BADARGS
        ;;
esac


EXAMPLES_PATH=$IN/examples
DOC_PATH=$IN/doc/html

SDK_DIR=maliit-sdk
SDK_OUT_PATH=$OUT/$SDK_DIR
SDK_OUT_FILE_PATH=$OUT/maliit-sdk-$VERSION.tar.gz

rm -r $SDK_OUT_PATH
mkdir -p $SDK_OUT_PATH/examples

# Extract examples
cp -r $EXAMPLES_PATH $SDK_OUT_PATH/
cd $SDK_OUT_PATH/examples
sed -i -e s/"^BUILD_TYPE.*=.*"/"BUILD_TYPE = $BUILD_TYPE"/ `grep -r --files-with-matches BUILD_TYPE ./ | tr "\n" " "`
make clean &> /dev/null
find ./ -name Makefile -exec rm {} \;
rm README
cd -

# Pack
cd $OUT
tar -acf $SDK_OUT_FILE_PATH $SDK_DIR
cd -

