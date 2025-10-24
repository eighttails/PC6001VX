#!/bin/bash

SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))
cd $SCRIPT_DIR

docker build . \
--build-arg UID="`id -u`" \
--build-arg GID="`id -g`" \
--build-arg HTTP_PROXY="$HTTP_PROXY" \
--build-arg HTTPS_PROXY="$HTTPS_PROXY" \
-t build-pc6001vx-appimage

docker run --privileged --rm -it \
    -v $SCRIPT_DIR/..:/work \
    build-pc6001vx-appimage \
    bash -c "cd /work/appimage && ./build_appimage.sh"
