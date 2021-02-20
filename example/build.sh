#!/bin/sh
BUILD_DIR="build"
rm -rf $BUILD_DIR
cmake -H. -B$BUILD_DIR
make -C $BUILD_DIR
