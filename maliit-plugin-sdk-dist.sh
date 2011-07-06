#!/bin/bash

### This script creates a tarball of the Maliit Plugin SDK for distribution

# TODO: 
# - a way to track which version of the framework the sdk is for
# - some sort of distcheck like option, that takes the resulting tarball
# and tries to compile it against the currently installed version
# - put into the build system?
VERSION=0.2.0

EXAMPLES_PATH=./examples
DOC_PATH=./doc/html
OUT_PATH=./maliit-plugin-sdk
SDK_FILE_PATH=./maliit-plugin-sdk-$VERSION.tar.gz

rm -r $OUT_PATH
mkdir -p $OUT_PATH/{documentation,examples}

# Examples
cp -r $EXAMPLES_PATH $OUT_PATH/
cd $OUT_PATH/examples
sed -i -e s/"^BUILD_TYPE.*=.*"/"BUILD_TYPE = skeleton"/ `grep -r --files-with-matches BUILD_TYPE ./ | tr "\n" " "`
make clean
find ./ -name Makefile -exec rm {} \;
rm README
cd -

# Documentation
cp -r $DOC_PATH $OUT_PATH/documentation

# Pack
tar -acf $SDK_FILE_PATH $OUT_PATH

