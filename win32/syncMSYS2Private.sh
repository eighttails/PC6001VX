#!/bin/bash
export SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))

#未コミットの変更があったら何もしない
if [ ! -z "$(git status --porcelain)"  ]; then 
echo Reposity is not clean.
exit 1
fi

cd $SCRIPT_DIR
git rm -r MSYS2Private
git clone https://github.com/eighttails/MSYS2Private.git
rm -rf MSYS2Private/.git*
git add MSYS2Private
git commit -m'MSYS2Privateを更新'


