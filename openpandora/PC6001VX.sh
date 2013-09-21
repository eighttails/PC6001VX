#!/bin/sh
SHDIR=`dirname $0`

export PATH=":\/home/tadahito/bin:/usr/local/bin:/usr/bin:/bin:/usr/local/games:/usr/games"
export LD_LIBRARY_PATH=":\/usr/lib:/lib:."
export HOME="/mnt/utmp/PC6001VX" XDG_CONFIG_HOME="/mnt/utmp/PC6001VX"

./PC6001VX
