#!/bin/bash

function prerequisite(){
#必要ライブラリ
pacman "${PACMAN_INSTALL_OPTS[@]}" \
$MINGW_PACKAGE_PREFIX-SDL2 \
$MINGW_PACKAGE_PREFIX-brotli \
$MINGW_PACKAGE_PREFIX-cc \
$MINGW_PACKAGE_PREFIX-clang \
$MINGW_PACKAGE_PREFIX-clang-tools-extra \
$MINGW_PACKAGE_PREFIX-dbus \
$MINGW_PACKAGE_PREFIX-gcc-libs \
$MINGW_PACKAGE_PREFIX-mlir \
$MINGW_PACKAGE_PREFIX-ninja \
$MINGW_PACKAGE_PREFIX-ntldd \
$MINGW_PACKAGE_PREFIX-openssl \
$MINGW_PACKAGE_PREFIX-pcre2 \
$MINGW_PACKAGE_PREFIX-pkgconf \
$MINGW_PACKAGE_PREFIX-polly \
$MINGW_PACKAGE_PREFIX-qtbinpatcher \
$MINGW_PACKAGE_PREFIX-vulkan-headers \
$MINGW_PACKAGE_PREFIX-vulkan-loader \
$MINGW_PACKAGE_PREFIX-xmlstarlet \
$MINGW_PACKAGE_PREFIX-zlib \
$MINGW_PACKAGE_PREFIX-zstd \
2> /dev/null

exitOnError

mkdir -p $PREFIX/bin 2> /dev/null
mkdir -p $QT6_STATIC_PREFIX/bin 2> /dev/null
pushd $MINGW_PREFIX/bin
cp -f $NEEDED_DLLS $QT6_STATIC_PREFIX/bin
popd
}

# Use the right mkspecs file
if [[ ${MINGW_PACKAGE_PREFIX} == *-clang-* ]]; then
  _platform=win32-clang-g++
else
  _platform=win32-g++
fi

# Helper macros to help make tasks easier #
apply_patch_with_msg() {
  for _patch in "$@"
  do
    echo "Applying ${_patch}"
    patch -Nbp1 -i "${SCRIPT_DIR}/${_patch}"
  done
}

function makeQtSourceTree(){
#Qt
QT_MAJOR_VERSION=6.2
QT_MINOR_VERSION=.1
QT_VERSION=$QT_MAJOR_VERSION$QT_MINOR_VERSION
QT_ARCHIVE_DIR=qt-everywhere-src-$QT_VERSION
QT_ARCHIVE=$QT_ARCHIVE_DIR.tar.xz
QT_SOURCE_DIR=qt6-src-$1
#QT_RELEASE=development_releases
QT_RELEASE=official_releases


if [ -e $QT_SOURCE_DIR ]; then
    # 存在する場合
    echo "$QT_SOURCE_DIR already exists."
else
    # 存在しない場合
    if [ ! -e $QT_ARCHIVE ]; then
    wget -c  http://download.qt.io/$QT_RELEASE/qt/$QT_MAJOR_VERSION/$QT_VERSION/single/$QT_ARCHIVE
    fi

    tar xf $QT_ARCHIVE
    mv $QT_ARCHIVE_DIR $QT_SOURCE_DIR
    pushd $QT_SOURCE_DIR

    apply_patch_with_msg \
        001-adjust-qmake-conf-mingw.patch \
        002-qt-6.2.0-win32-g-Add-QMAKE_EXTENSION_IMPORTLIB-defaulting-to-.patch \
        003-qt-6.2.0-dont-add-resource-files-to-qmake-libs.patch \
        004-Allow-overriding-CMAKE_FIND_LIBRARY_SUFFIXES-to-pref.patch \
        005-qt-6.2.0-win32static-cmake-link-ws2_32-and--static.patch \
        006-Fix-finding-D-Bus.patch \
        007-Fix-using-static-PCRE2-and-DBus-1.patch \
        008-Fix-libjpeg-workaround-for-conflict-with-rpcndr.h.patch \
        009-Fix-transitive-dependencies-of-static-libraries.patch \
        010-Support-finding-static-MariaDB-client-library.patch \
        011-Fix-crashes-in-rasterization-code-using-setjmp.patch \
        012-Handle-win64-in-dumpcpp-and-MetaObjectGenerator-read.patch \
        013-qmng-fix-build.patch \
        014-fix-relocatable-prefix-staticbuild-v2.patch \
        015-qt6-windeployqt-fixes.patch

    local _ARCH_TUNE=
    local _HARD_FLAGS=
    case ${MINGW_CHOST} in
    i686*)
        _ARCH_TUNE="-march=i686 -mtune=core2"
        if [ "${_enable_hardening}" = "yes" ]; then
        _HARD_FLAGS="-Wl,--dynamicbase,--nxcompat,--no-seh"
        fi
        ;;
    x86_64*)
        _ARCH_TUNE="-march=nocona -mtune=core2"
        if [ "${_enable_hardening}" = "yes" ]; then
        _HARD_FLAGS="-Wl,--dynamicbase,--high-entropy-va,--nxcompat,--default-image-base-high"
        fi
        ;;
    esac

    BIGOBJ_FLAGS="-Wa,-mbig-obj"

    # Append these ones ..
    sed -i "s|^QMAKE_CFLAGS .*= \(.*\)$|QMAKE_CFLAGS            = \1 ${_ARCH_TUNE} ${BIGOBJ_FLAGS} ${LTCG_CFLAGS}|g" qtbase/mkspecs/${_platform}/qmake.conf
    sed -i "s|^QMAKE_LFLAGS           +=\(.*\)$|QMAKE_LFLAGS            += \1 ${_HARD_FLAGS}|g" qtbase/mkspecs/common/gcc-base.conf

    popd
fi

}


function buildQtStatic(){
if [ -e $QT6_STATIC_PREFIX/bin/qmake.exe -a $((FORCE_INSTALL)) == 0 ]; then
    echo "Qt6 Static Libs are already installed."
    return 0
fi

#Qtのソースコードを展開
makeQtSourceTree static
exitOnError

#static版
QT6_STATIC_BUILD=qt6-static-$BIT
rm -rf $QT6_STATIC_BUILD
mkdir $QT6_STATIC_BUILD
pushd $QT6_STATIC_BUILD


MSYS2_ARG_CONV_EXCL="-DCMAKE_INSTALL_PREFIX=;-DCMAKE_CONFIGURATION_TYPES=;-DCMAKE_FIND_LIBRARY_SUFFIXES=" \
cmake \
    -G "Ninja Multi-Config" \
    -DCMAKE_CONFIGURATION_TYPES="Release" \
    -DCMAKE_FIND_LIBRARY_SUFFIXES_OVERRIDE=".a;.dll.a" \
    -DCMAKE_EXE_LINKER_FLAGS="${LDFLAGS} -static -static-libgcc -static-libstdc++" \
    -DBUILD_SHARED_LIBS=OFF \
    -DQT_QMAKE_TARGET_MKSPEC=${_platform} \
    -DCMAKE_INSTALL_PREFIX=$(cygpath -am $QT6_STATIC_PREFIX) \
    -DINSTALL_BINDIR=bin \
    -DINSTALL_LIBDIR=lib \
    -DINSTALL_INCLUDEDIR=include/qt6 \
    -DINSTALL_ARCHDATADIR=share/qt6 \
    -DINSTALL_DOCDIR=share/doc/qt6 \
    -DINSTALL_DATADIR=share/qt6 \
    -DINSTALL_MKSPECSDIR=share/qt6/mkspecs \
    -DINSTALL_DESCRIPTIONSDIR=share/qt6/modules \
    -DINSTALL_TESTSDIR=share/qt6/tests \
    -DINSTALL_EXAMPLESDIR=share/doc/qt6/examples \
    -DFEATURE_static_runtime=ON \
    -DFEATURE_relocatable=ON \
    -DFEATURE_openssl_linked=ON \
    -DINPUT_openssl=linked \
    -DINPUT_dbus=linked \
    -DINPUT_mng=yes \
    -DINPUT_libmd4c=qt \
    -DFEATURE_glib=OFF \
    -DINPUT_qt3d_assimp=qt \
    -DINPUT_quick3d_assimp=qt \
    -DFEATURE_system_assimp=OFF \
    -DFEATURE_system_doubleconversion=OFF \
    -DFEATURE_system_freetype=OFF \
    -DFEATURE_system_harfbuzz=OFF \
    -DFEATURE_hunspell=OFF \
    -DFEATURE_system_hunspell=OFF \
    -DFEATURE_mng=OFF \
    -DFEATURE_jasper=OFF \
    -DFEATURE_system_jpeg=OFF \
    -DFEATURE_system_pcre2=OFF \
    -DFEATURE_system_mng=OFF \
    -DFEATURE_system_png=OFF \
    -DFEATURE_system_sqlite=OFF \
    -DFEATURE_system_tiff=OFF \
    -DFEATURE_system_webp=OFF \
    -DFEATURE_system_zlib=OFF \
    -DFEATURE_opengl=ON \
    -DFEATURE_opengl_dynamic=ON \
    -DFEATURE_opengl_desktop=OFF \
    -DFEATURE_egl=OFF \
    -DFEATURE_gstreamer=OFF \
    -DFEATURE_icu=OFF \
    -DFEATURE_fontconfig=OFF \
    -DFEATURE_pkg_config=ON \
    -DFEATURE_vulkan=ON \
    -DFEATURE_sql_ibase=OFF \
    -DFEATURE_sql_psql=OFF \
    -DFEATURE_sql_mysql=OFF \
    -DFEATURE_sql_odbc=OFF \
    -DFEATURE_zstd=OFF \
    -DFEATURE_wmf=ON \
    -DQT_BUILD_TESTS=OFF \
    -DQT_BUILD_EXAMPLES=OFF \
    -DBUILD_qttools=ON \
    -DBUILD_qtdoc=OFF \
    -DBUILD_qttranslations=ON \
    -DBUILD_qtwebengine=OFF \
    -DOPENSSL_DEPENDENCIES="-lws2_32;-lgdi32;-lcrypt32" \
    -DPOSTGRESQL_DEPENDENCIES="-lpgcommon;-lpgport;-lintl;-lssl;-lcrypto;-lshell32;-lws2_32;-lsecur32;-liconv" \
    -DMYSQL_DEPENDENCIES="-lssl;-lcrypto;-lshlwapi;-lgdi32;-lws2_32;-lpthread;-lz;-lm" \
    -DLIBPNG_DEPENDENCIES="-lz" \
    -DGLIB2_DEPENDENCIES="-lintl;-lws2_32;-lole32;-lwinmm;-lshlwapi;-lm" \
    -DFREETYPE_DEPENDENCIES="-lbz2;-lharfbuzz;-lfreetype;-lbrotlidec;-lbrotlicommon" \
    -DHARFBUZZ_DEPENDENCIES="-lglib-2.0;-lintl;-lws2_32;;-lgdi32;-lole32;-lwinmm;-lshlwapi;-lintl;-lm;-lfreetype;-lgraphite2;-lrpcrt4" \
    -DLIBBROTLIDEC_DEPENDENCIES="-lbrotlicommon" \
    -DLIBBROTLIENC_DEPENDENCIES="-lbrotlicommon" \
    -DLIBBROTLICOMMON_DEPENDENCIES="" \
    -DDBUS1_DEPENDENCIES="-lws2_32;-liphlpapi;-ldbghelp" \
    $(cygpath -am ../$QT_SOURCE_DIR)

export PATH=$PWD/bin:$PATH

cmake --build .
exitOnError

cmake --install .
exitOnError

popd

rm -rf $QT6_STATIC_BUILD
}



#----------------------------------------------------
SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))
source $SCRIPT_DIR/../common/common.sh
commonSetup

#必要ライブラリ
prerequisite

export PKG_CONFIG="$(cygpath -am $MINGW_PREFIX/bin/pkg-config.exe)"
export LLVM_INSTALL_DIR=$(cygpath -am $MINGW_PREFIX)

#Qtのインストール場所
QT6_STATIC_PREFIX=$PREFIX/qt6-static-private

cd $EXTLIB

#static版Qtをビルド
buildQtStatic
exitOnError

