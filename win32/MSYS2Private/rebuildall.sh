#!/bin/bash
SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))
rm -rf $MINGW_PREFIX/local
cd $SCRIPT_DIR
./buildall.sh
