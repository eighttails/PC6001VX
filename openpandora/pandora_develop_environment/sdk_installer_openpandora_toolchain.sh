#!/bin/bash
#
# openpandora_toolchain.sh:
# Script for installing a crosscompiler toolchain that allows building binaries
# for the Pandora.
#
# Features:
# * Rather easy to setup toolchain that allows compiling binaries for the
#   pandora on a linux host.
# * Should support x86 as well as amd64 with multilib support.
# * Installable inside a users $HOME without root priviledges.
# * Completely based upon the packages that are also used on the pandora. It
#   should use exactly the versions as shipped with the pandora and just those.
# * Includes all libs and headers that are also found on the pandora (might
#   even include some useless things) but only a small number of binaries that
#   can't be run on the host architecture.
# * Should be rather easy to keep in sync with upsteam (aka openpandora.org)
#   version changes simply by rerunning the script.
#
# The latest version of this script should be available in the openpandora.org
# git repository:
# http://git.openpandora.org/cgi-bin/gitweb.cgi?p=pandora-misc.git;a=tree;f=sdk_installer
# 
# Some more (partly historical) information about this installer script is
# available in the following forum thread:
# http://www.gp32x.com/board/index.php?/topic/58443-crosscompiler-toolchain-based-on-openpandora-org-ipks/
#
# This script was initially based upon the scripts provided in these threads:
# http://www.gp32x.com/board/index.php?/topic/52343-pandora-sdk-installer/
# http://www.gp32x.com/board/index.php?/topic/55453-pandora-sdk-installer-with-opkg/
#
# Copyright (C) 2011, 2012 by Nils Kneuper <crazy-ivanovic@gmx.net>
#
#
# How to use:
# * Adjust the variable "PNDSDK_DIR" so that it shows a valid path for your
#   system. The folder you give it there should have write permissions for your
#   current user. Otherwise things will not work!
# * If you downloaded the script from a pastebin site you should apply
#   "dos2unix" on it to make sure that it works!
# * Run the script without root priviledges. If you run it with root privs it
#   might hurt your system. Don't blame me if you destroy your system with this
#   script, you have been warned!
#
# Required dependencies:
# * You need a working compile environment on your host system to process
#   several of the steps. This has to be created before running the script.
#   Besides you will need some additional programs like eg wget and some
#   others. Existance of those inside $PATH will be checked and you will get an
#   error message if they don't exist.
# * You need about 2.0GB for the toolchain plus about 700MB for temporary
#   files. Those 700MB will be downloaded using your internetconnection.
#
# Known limitations:
# * This toolchain won't work on a pure 64bit system (common systems also
#   install 32bit libs, so this most likely does not affect you!).
# * Whitespaces as well as quotes won't work inside the path to your PNDSDK_DIR.
# * It will not automagically install the deps you need later on to run it, some
#   hints what might be required are at the end of this comment section.
# * The files installed by this script (mainly the codesourcery based toolchain
#   will be owned by the current user. Because of this and depending on your
#   system setup the toolchain might only work for this very user (not 100% sure
#   since it all depends on your system setup...).
# * Some things like eg the gettext tools (for generation of translations used
#   in the programs you compile with your toolchain) will have to be provided by
#   your system. Stuff like msgfmt is *not* included!
# * This toolchain can most likely not be used to build complete pandora (NAND)
#   images. For this you still have to get an OpenEmbedded toolchain.
#
#
# Getting some of the required dependencies in "Debian" style (usable by
# debian derivates like Ubuntu, too!):
# apt-get install libcurl4-openssl-dev libgpgme11-dev libtool build-essential
#
# Getting the deps for Gentoo:
# * If you are using amd64 make sure that the useflag "multilib" is selected.
# * All required dependencies for this script to work should already be
#   available.
#
#
# TODO:
# * Allow calling the script with parameters, so that just specific parts are
#   executed (and add stuff like --help as well as defining the PNDSDK_DIR as
#   parameter)
# * Make it possible to continue the script in case it had to be aborted before
# * Add some more useful files like eg some "set environment vars" script
# * Make fix_libs() nicer, currently it is not as clean as it could/should be
# * Fix any issues reported ;)
#
#
# This script is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 or,
# at your option any later version. This program is distributed in the
# hope that it will be useful, but WITHOUT ANY WARRANTY. See 
# http://www.gnu.org/licenses/old-licenses/gpl-2.0.html for more details.


# Edit the following line so that it matches the place where you want the SDK
# installed.
PNDSDK_DIR=$HOME/pandora-dev


# Don't edit the line below, it depends on the codesourcery toolchain!
#PNDSDK=$PNDSDK_DIR/arm-2009q3
#PNDSDK=$PNDSDK_DIR/arm-2010.09
#PNDSDK=$PNDSDK_DIR/arm-2011.03
PNDSDK=$PNDSDK_DIR/arm-2011.09
#PNDSDK=$PNDSDK_DIR/arm-2013.11
USEFUL_TOOLS_DIR=$PNDSDK_DIR/sdk_utils

TARGET_SYS=arm-none-linux-gnueabi

# Programs to download and their checksums.
#TOOLCHAIN_URL=http://www.codesourcery.com/sgpp/lite/arm/portal/package5383/public/arm-none-linux-gnueabi/arm-2009q3-67-arm-none-linux-gnueabi-i686-pc-linux-gnu.tar.bz2
#TOOLCHAIN_TARBALL_NAME=arm-2009q3-67-arm-none-linux-gnueabi-i686-pc-linux-gnu.tar.bz2
#TOOLCHAIN_MD5=2c2b324f0a84916afd9920029286eb10
#TOOLCHAIN_URL=http://www.codesourcery.com/sgpp/lite/arm/portal/package7851/public/arm-none-linux-gnueabi/arm-2010.09-50-arm-none-linux-gnueabi-i686-pc-linux-gnu.tar.bz2
#TOOLCHAIN_TARBALL_NAME=arm-2010.09-50-arm-none-linux-gnueabi-i686-pc-linux-gnu.tar.bz2
#TOOLCHAIN_MD5=f9dbd7a2daf20724e013cc4b5b64d62f
#TOOLCHAIN_URL=http://www.codesourcery.com/sgpp/lite/arm/portal/package8739/public/arm-none-linux-gnueabi/arm-2011.03-41-arm-none-linux-gnueabi-i686-pc-linux-gnu.tar.bz2
#TOOLCHAIN_TARBALL_NAME=arm-2011.03-41-arm-none-linux-gnueabi-i686-pc-linux-gnu.tar.bz2
#TOOLCHAIN_MD5=63d93a9f7fe9ff007bc69adfadc7c4ab
TOOLCHAIN_URL=http://sourcery.mentor.com/sgpp/lite/arm/portal/package9728/public/arm-none-linux-gnueabi/arm-2011.09-70-arm-none-linux-gnueabi-i686-pc-linux-gnu.tar.bz2
TOOLCHAIN_TARBALL_NAME=arm-2011.09-70-arm-none-linux-gnueabi-i686-pc-linux-gnu.tar.bz2
TOOLCHAIN_MD5=56d9a77654d012914e703dc8d9383246

#TOOLCHAIN_URL=https://sourcery.mentor.com/GNUToolchain/package12225/public/arm-none-linux-gnueabi/arm-2013.11-33-arm-none-linux-gnueabi-i686-pc-linux-gnu.tar.bz2
#TOOLCHAIN_TARBALL_NAME=arm-2013.11-33-arm-none-linux-gnueabi-i686-pc-linux-gnu.tar.bz2
#TOOLCHAIN_MD5=56276ed5d7a8edffa9d536a18284e5e0


OPKG_VERSION=opkg-0.1.8
OPKG_ARCHIVE_MD5=c714ce0e4863bf1315e3b6913ffe3299
OPKG_ARCHIVE=$OPKG_VERSION.tar.gz
#PKGCONFIG_VERSION=pkg-config-0.25
#PKGCONFIG_ARCHIVE_MD5=a3270bab3f4b69b7dc6dbdacbcae9745
PKGCONFIG_VERSION=pkg-config-0.26
PKGCONFIG_ARCHIVE_MD5=47525c26a9ba7ba14bf85e01509a7234
PKGCONFIG_ARCHIVE=$PKGCONFIG_VERSION.tar.gz


CWD=[ pwd ]
# As default the temporary directory is always a subdir of the current folder,
# not sure if this should be move further to the top and be "userdefinable"...
TMPDIR=`pwd`/tmp

REQUIRED_PROGS="wget md5sum tar make cut grep sed find awk xargs chmod"

# useful functions ...
black='\E[30m'
red='\E[31m'
green='\E[32m'
yellow='\E[33m'
blue='\E[34m'
magenta='\E[35m'
cyan='\E[36m'
white='\E[37m'


cecho ()	# Color-echo. Argument $1 = message, Argument $2 = color
{
	local default_msg="No message passed."   # Doesn't really need to be a local variable.
	message=${1:-$default_msg}               # Defaults to default message.
	color=${2:-$black}                       # Defaults to black, if not specified.
	echo -e "$color $message"
	tput sgr0                                # Reset to normal.
	return
} 


check_wget_error()
{
	if [ "$?" -ne "0" ];
	then
		cecho "ERROR: Sorry, wget failed. The download url seems to not work. Please retry later." $red
		exit 1
	fi
}


check_error()
{
	if [ "$?" -ne "0" ];
	then
		cecho "ERROR: Sorry, some error occured. Stopping this script now. Please check for the error message above (eg. 'not enough space left') and fix them before retrying the script." $red
		exit 1
	fi
}


check_md5sum()
{
	LOCAL_ARCHIV=${1}
	LOCAL_MD5=${2}
	echo -e
	cecho "Verifying the integrity of $LOCAL_ARCHIV" $green
	md5=$(md5sum $TMPDIR/$LOCAL_ARCHIV |cut -d' ' -f1)
	if [ "$md5" != "$LOCAL_MD5" ]
	then
		cecho "ERROR: $TMPDIR/$LOCAL_ARCHIV is corrupted, please fix" $red
		exit 1
	fi
}


report_absent_tool()
{
    cecho "ERROR: $1 is not present in PATH. $(basename ${0}) requires it in order to work properly."  $red
    exit 1
}


download_codesourcery_toolchain()
{
	# Check if codesourcery TOOLCHAIN is available, download it if needed
	echo -e
	cecho "Checking if codesourcery toolchain is available, download it if needed" $green
	if [ ! -f "$TMPDIR/$TOOLCHAIN_TARBALL_NAME" ]
	then
		#slow url directly at codesourcery
		#wget -O $TMPDIR/$TOOLCHAIN http://www.codesourcery.com/sgpp/lite/arm/portal/package5383/public/arm-none-linux-gnueabi/$TOOLCHAIN
		#working url for the 2009q3 toolchain using the google servers that tend to be faster...
		#wget -O $TMPDIR/$TOOLCHAIN http://smp-on-qemu.googlecode.com/files/$TOOLCHAIN
		#working url for the 2010.09 toolchain
		wget -O $TMPDIR/$TOOLCHAIN_TARBALL_NAME $TOOLCHAIN_URL
		check_wget_error
	fi
}


install_codesourcery_toolchain()
{
	# Verify the codesourcery toolchain integrity
	check_md5sum $TOOLCHAIN_TARBALL_NAME $TOOLCHAIN_MD5

	# Create the pandora sdk directory
	echo -e
	cecho "Creating the pandora sdk directory ($PNDSDK_DIR)" $green
	if [ ! -d $PNDSDK_DIR ]
	then
			mkdir -p $PNDSDK_DIR
			chown -R $USER $PNDSDK_DIR
	else
			chown -R $USER $PNDSDK_DIR
	fi

	# Extract codesourcery toolchain
	echo -e
	cecho "Extracting codesourcery toolchain to $PNDSDK_DIR" $green
	tar -xjf $TMPDIR/$TOOLCHAIN_TARBALL_NAME -C $PNDSDK_DIR
	check_error

	#move libstdc++ files that are going to just hurt to some place where they should not be a bother...
	#the version later on installed by the libstdc++ package should be perfectly fine!
	echo -e
	cecho "Moving the broken (in case of the pandora!) libstdc++.so files out of the way" $green
	for i in $(find $PNDSDK/arm-none-linux-gnueabi/ -name 'libstdc++.so*'); do mv $i $i.backup; done
}


download_opkg()
{
	echo -e
	cecho "Checking if opkg is available, download it if needed" $green
	if [ ! -f "$TMPDIR/$OPKG_ARCHIVE" ]
	then
		wget -O $TMPDIR/$OPKG_ARCHIVE http://opkg.googlecode.com/files/$OPKG_ARCHIVE
		check_wget_error
	fi
}


install_opkg()
{
	# Verify the opkg integrity
	check_md5sum $OPKG_ARCHIVE $OPKG_ARCHIVE_MD5

	echo -e
	cecho "Extracting opkg source tarball to $TMPDIR/$OPKG_VERSION" $green
	# First remove the old compile dir since it might clash with the new one if 
	# some things (paths!) are changed!
	if [ -d "$TMPDIR/$OPKG_VERSION" ]
	then
		rm -rf $TMPDIR/$OPKG_VERSION
	fi
	tar -xzf $TMPDIR/$OPKG_ARCHIVE -C $TMPDIR/
	
	echo -e
	cecho "Configuring, compiling and installing opkg into your SDK" $green
	cd $TMPDIR/$OPKG_VERSION
	./configure --prefix=$PNDSDK --with-opkglockfile=/tmp/opkg_lock --with-opkgetcdir=$PNDSDK/
	if [ "$?" -ne "0" ];
	then
		cecho "ERROR: Configure for opkg failed, please check the output above" $red
		exit 1
	fi
	make && make install
	if [ "$?" -ne "0" ];
	then
		cecho "ERROR: make && make install for opkg failed, please check the output above" $red
		exit 1
	fi
	cd $CWD

	echo -e
	cecho "Creating config files and folders required for opkg" $green

	mkdir -p $PNDSDK/lists_dir
	mkdir -p $PNDSDK/tmp

	#create config file:
	cat > $PNDSDK/opkg.conf <<_END_
#arch all 1
#arch any 6
arch noarch 11
#arch arm 16
#arch armv4 21
#arch armv4t 26
#arch armv5te 31
#arch armv6 36
#arch armv7 41
arch armv7a 46
arch omap3-pandora 51

# Must have one or more source entries of the form:
#
#   src <src-name> <source-url>
#
# and one or more destination entries of the form:
#
#   dest <dest-name> <target-path>
#
# where <src-name> and <dest-names> are identifiers that
# should match [a-zA-Z0-9._-]+, <source-url> should be a
# URL that points to a directory containing a Familiar
# Packages file, and <target-path> should be a directory
# that exists on the target system.

# Proxy Support
#option http_proxy http://proxy.tld:3128
#option ftp_proxy http://proxy.tld:3128
#option proxy_username <username>
#option proxy_password <password>

# Enable GPGME signature
# option check_signature 1

# Offline mode (for use in constructing flash images offline)
#option offline_root target

dest root /
lists_dir ext $PNDSDK/lists_dir



src/gz base http://www.openpandora.org/feeds/unstable/armv7a/

src/gz no-arch http://www.openpandora.org/feeds/unstable/all

src/gz omap3-pandora http://www.openpandora.org/feeds/unstable/omap3-pandora


_END_

	echo -e
	cecho "Installation of opkg completed" $green
}


download_pkgconfig()
{
	echo -e
	cecho "Checking if pkg-config is available, download it if needed" $green
	if [ ! -f "$TMPDIR/$PKGCONFIG_ARCHIVE" ]
	then
		wget -O $TMPDIR/$PKGCONFIG_ARCHIVE http://pkgconfig.freedesktop.org/releases/$PKGCONFIG_ARCHIVE 
		check_wget_error
	fi
}


install_pkgconfig()
{
	# Verify the opkg integrity
	check_md5sum $PKGCONFIG_ARCHIVE $PKGCONFIG_ARCHIVE_MD5

	echo -e
	cecho "Extracting pkg-config source tarball to $TMPDIR/$PKGCONFIG_VERSION" $green
	# First remove the old compile dir since it might clash with the new one if 
	# some things (paths!) are changed!
	if [ -d "$TMPDIR/$PKGCONFIG_VERSION" ]
	then
		rm -rf $TMPDIR/$PKGCONFIG_VERSION
	fi
	tar -xzf $TMPDIR/$PKGCONFIG_ARCHIVE -C $TMPDIR/
	
	echo -e
	cecho "Configuring, compiling and installing pkg-config into your SDK" $green
	cd $TMPDIR/$PKGCONFIG_VERSION
	./configure --libdir=$PNDSDK/usr/lib --program-prefix=arm-none-linux-gnueabi- --prefix=$PNDSDK
	if [ "$?" -ne "0" ];
	then
		cecho "ERROR: Configure for pkg-config failed, please check the output above" $red
		exit 1
	fi
	make && make install
	if [ "$?" -ne "0" ];
	then
		cecho "ERROR: make && make install for pkg-config failed, please check the output above" $red
		exit 1
	fi
	cd $CWD
}


gather_package_list()
{
	echo -e
	cecho "Fetching a package list to use for installing the libs" $green
	# General idea in this section:
	# 1) Get a list with *all* feeds that openpandora.org does have.
	# 2) Get the plain package names from those lists. Those package names will
	#    be used as input for opkg later on.
	# 3) remove some "known to be useless" entries trying to only keep all
	#    devel related packages as well as the static packages.
	
	# Get stamp files from all relevant folders. If new feeds are added, this
	# section has to be extended.
	wget --output-document=$TMPDIR/Packages.stamps.all http://www.openpandora.org/feeds/unstable/all/Packages.stamps
	wget --output-document=$TMPDIR/Packages.stamps.armv7a http://www.openpandora.org/feeds/unstable/armv7a/Packages.stamps
	wget --output-document=$TMPDIR/Packages.stamps.omap3-pandora http://www.openpandora.org/feeds/unstable/omap3-pandora/Packages.stamps
	
	# Transform the list into package names only and sort the list alphabetically.
	cat $TMPDIR/Packages.stamps.* | cut -f 2 -d " " | cut -f 1 -d "_" | sort > $TMPDIR/packages.txt.tmp1
	# Remove duplicate entries.
	awk '!x[$0]++' $TMPDIR/packages.txt.tmp1 > $TMPDIR/packages.txt.tmp2
	
	# Remove lots of useless stuff (useless for the sdk!) as well as missing
	# things. If more packages should be removed, extend the list of "grep -v"
	# entries.
	cat $TMPDIR/packages.txt.tmp2 | grep -E "\-dev$|\-devel$|\-dev\-|\-static$" | \
		grep -v "^abiword" | \
		grep -v "^libgoffice" | \
		grep -v "^claws-mail" | \
		grep -v "^gnumeric" | \
		grep -v "^pidgin" | \
		grep -v "^xfwm4-theme" | \
		grep -v "^u-boot-omap3pandora-dev" | \
		grep -v "^asio-dev" | \
		grep -v "^dhcp-dev" | \
		grep -v "^libnet-dev" | \
		grep -v "^dropbear-dev" | \
		grep -v "^libstdc++-dev" | \
		grep -v "^angstrom-feed-configs-dev" | \
		grep -v "^coreutils-dev" | \
		grep -v "^mesa" \
		> $TMPDIR/packages.txt
	
	# Cleanup.
	rm $TMPDIR/packages.txt.tmp* $TMPDIR/Packages.stamps.*
	
	echo -e
	cecho "Finished creation of $TMPDIR/packages.txt" $green
}


install_libs_via_opkg()
{
	echo -e
	cecho "Updating opkg package feeds" $green
	#update repository against latest version
	$PNDSDK/bin/opkg-cl --conf=$PNDSDK/opkg.conf --offline-root=$PNDSDK update
	check_error

	echo -e
	cecho "Installing all libs, based on $TMPDIR/packages.txt" $green
	cecho "Error messages from opkg about missing dependencies can be ignored!" $cyan
	#install all possible dev packages adding their deps, too
	$PNDSDK/bin/opkg-cl --conf=$PNDSDK/opkg.conf --cache=$TMPDIR --offline-root=$PNDSDK install `cat $TMPDIR/packages.txt | cut -f 1 -d " " | xargs echo`
	check_error
	cecho "The error messages from opkg about missing dependencies can be ignored!" $cyan

	echo -e
	cecho "Removing packages that break the toolchain otherwise" $green
	#"libstdc++-dev" does always get installed but it will break things, so directly remove it again...
	$PNDSDK/bin/opkg-cl --conf=$PNDSDK/opkg.conf --offline-root=$PNDSDK remove libstdc++-dev

	echo -e
	cecho "Removing ARM binaries from the host" $green
	# After installing all those lovely programs, remove all files that are
	# inside a "bin/" folder and belong to arm architecture.
	# Steps taken for this:
	# 1) find all files inside the installed sdk with "bin/" in their path
	# 2) call "file" on each file
	# 3) get all files that include ", ARM," in the output, those only run on 
	#    the target arch, not on the host system
	# 4) filter out things that are part of the codesourcery sdk, no idea what
	#    those files with ARM arch are included, but it might have a reason
	# 5) only use the filename of the output
	# 6) get rid of the offending files using rm via xargs
	find $PNDSDK -wholename "*/bin/*" -exec file '{}' \; | grep ", ARM," | grep -v "arm-none-linux-gnueabi" | cut -f1 -d ":" | xargs rm

	echo -e
	cecho "Removing broken symlinks" $green
	# Find broken symlinks and remove them; since the sdk works and the links
	# *are* not working, removing them should not cause further issues...
	# Based upon http://www.zulustips.com/2007/01/26/how-to-find-broken-symlinks.html
	for i in `find $PNDSDK/ -type l`; do [ -e $i ] || rm $i; done


	echo -e
	cecho "Completed installation of libraries via opkg" $green
}


fix_libs()
{
	echo -e
	cecho "Fixing libs and symlinks" $green
	
	# In case the libpng-config is missing (this is currently the case!) create
	# one ourselves...
	if [ ! -f "$PNDSDK/usr/bin/libpng-config" ]
	then
		cecho "No $PNDSDK/usr/bin/libpng-config found, creating one" $green
		cat > $PNDSDK/usr/bin/libpng-config <<_END_
#! /bin/sh

# libpng-config
# provides configuration info for libpng.

# Copyright (C) 2002, 2004, 2006, 2007 Glenn Randers-Pehrson

# This code is released under the libpng license.
# For conditions of distribution and use, see the disclaimer
# and license in png.h

# Modeled after libxml-config.

version="1.2.42"
prefix="$PNDSDK/usr"
exec_prefix="\${prefix}"
libdir="$PNDSDK/usr/lib"
includedir="\${prefix}/include/libpng12"
libs="-lpng12"
all_libs="-lpng12 -lz -lm "
I_opts="-I\${includedir}"
L_opts="-L\${libdir}"
R_opts=""
cppflags=""
ccopts=""
ldopts=""

usage()
{
    cat <<EOF
Usage: \$0 [OPTION] ...

Known values for OPTION are:

  --prefix        print libpng prefix
  --libdir        print path to directory containing library
  --libs          print library linking information
  --ccopts        print compiler options
  --cppflags      print pre-processor flags
  --cflags        print preprocessor flags, I_opts, and compiler options
  --I_opts        print "-I" include options
  --L_opts        print linker "-L" flags for dynamic linking
  --R_opts        print dynamic linker "-R" or "-rpath" flags
  --ldopts        print linker options
  --ldflags       print linker flags (ldopts, L_opts, R_opts, and libs)
  --static        revise subsequent outputs for static linking
  --help          print this help and exit
  --version       print version information
EOF

    exit \$1
}

if test \$# -eq 0; then
    usage 1
fi

while test \$# -gt 0; do
    case "\$1" in

    --prefix)
        echo \${prefix}
        ;;

    --version)
        echo \${version}
        exit 0
        ;;

    --help)
        usage 0
        ;;

    --ccopts)
        echo \${ccopts}
        ;;

    --cppflags)
        echo \${cppflags}
        ;;

    --cflags)
        echo \${I_opts} \${cppflags} \${ccopts}
        ;;

    --libdir)
        echo \${libdir}
        ;;

    --libs)
        echo \${libs}
        ;;

    --I_opts)
        echo \${I_opts}
        ;;

    --L_opts)
        echo \${L_opts}
        ;;

    --R_opts)
        echo \${R_opts}
        ;;

    --ldopts)
    	echo \${ldopts}
	;;

    --ldflags)
        echo \${ldopts} \${L_opts} \${R_opts} \${libs}
        ;;

    --static)
        R_opts=""
	libs=\${all_libs}
        ;;

    *)
        usage
        exit 1
        ;;
    esac
    shift
done

exit 0

_END_
		chmod +x $PNDSDK/usr/bin/libpng-config
		ln -s $PNDSDK/usr/bin/libpng-config $PNDSDK/usr/bin/libpng12-config
	fi

	# In case the lua5.1.pc is missing (this is currently the case!) create
	# one ourselves...
	if [ ! -f "$PNDSDK/usr/lib/pkgconfig/lua5.1.pc" ]
	then
		cecho "No $PNDSDK/usr/lib/pkgconfig/lua5.1.pc found, creating one" $green
		cat > $PNDSDK/usr/lib/pkgconfig/lua5.1.pc <<_END_
# lua.pc -- pkg-config data for Lua

# vars from install Makefile

# grep '^V=' ../Makefile
V= 5.1

# grep '^INSTALL_.*=' ../Makefile | sed 's/INSTALL_TOP/prefix/'
prefix=$PNDSDK/usr
INSTALL_BIN= \${prefix}/bin
INSTALL_INC= \${prefix}/include
INSTALL_LIB= \${prefix}/lib
INSTALL_MAN= \${prefix}/man/man1
INSTALL_LMOD= \${prefix}/share/lua/\${V}
INSTALL_CMOD= \${prefix}/lib/lua/\${V}

exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib
includedir=\${prefix}/include

Name: Lua
Description: An Extensible Extension Language
Version: \${V}
Requires: 
Libs: -L\${libdir} -llua -lm
Cflags: -I\${includedir}

_END_
	fi

	# update library/include paths
	ln -s $PNDSDK/usr/bin/sdl-config $PNDSDK/usr/bin/arm-none-linux-gnueabi-sdl-config
	ln -s $PNDSDK/usr/bin/libpng-config $PNDSDK/usr/bin/arm-none-linux-gnueabi-libpng-config
	ln -s $PNDSDK/usr/bin/freetype-config $PNDSDK/usr/bin/arm-none-linux-gnueabi-freetype-config

	# add some symlinks so that the compiler can eg be called as "pandora-gcc"
	# (requested by JayFoxRox in #openpandora)
	for i in $(find $PNDSDK -wholename "*bin/$TARGET_SYS*")
	do
		if [ ! -d $i ];then
			ln -s $i `echo $i|sed "s,$TARGET_SYS,pandora,g"`
		fi
	done

	# For compilation using qmake "plain" .so files are required for qt4, by
	# default only libQT*.so.4 (and more precise version numbers) are
	# installed. Adding symlinks to work around this problem.
	for i in $(find $PNDSDK -name "libQt*.so.4")
	do
		ln -s $i `echo $i | sed "s#\.so\.4#\.so#g"`
	done

	#SEDCMD="s#/usr/local/pandora#$PNDSDK_DIR#g;s#prefix=\/.*#prefix=$PNDSDK/usr#g;s#includedir=\/.*#includedir=$PNDSDK/usr/include#g;s#libdir=\/.*#libdir='$PNDSDK/usr/lib'#;s#pluginsdir=/usr/lib/\(.*\)#pluginsdir='$PNDSDK/usr/lib/\1'#;s# /usr/lib/lib\([A-Z0-9a-z.-]*\).la# $PNDSDK/usr/lib/lib\1.la#g;s# /usr/lib# $PNDSDK/usr/lib#g;s#-L/usr/lib#-L$PNDSDK/usr/lib#g;s#-R/usr/lib#-R$PNDSDK/usr/lib#g;s#-I/usr/include#-I$PNDSDK/usr/include#g;s#-isystem\/.*\b##g"
	SEDCMD="s#/usr/local/pandora#$PNDSDK_DIR#g;s#prefix=\/.*#prefix=$PNDSDK/usr#g;s#includedir=/usr/include\(.*\)#includedir=$PNDSDK/usr/include\1#;s#libdir=/usr/lib\(.*\)#libdir=$PNDSDK/usr/lib\1#;s#pluginsdir=/usr/lib\(.*\)#pluginsdir=$PNDSDK/usr/lib\1#;s# /usr/lib/lib\([A-Z0-9a-z.-]*\).la# $PNDSDK/usr/lib/lib\1.la#g;s# /usr/lib# $PNDSDK/usr/lib#g;s#-L/usr/lib#-L$PNDSDK/usr/lib#g;s#-R/usr/lib#-R$PNDSDK/usr/lib#g;s#-I/usr/include#-I$PNDSDK/usr/include#g;s#-isystem\/.*\b##g"

	cecho "Fixing paths inside *la, *pc and files in usr/bin/" $green
	for i in $(find $PNDSDK/usr/lib -name '*la'; find $PNDSDK/usr/lib/pkgconfig -name '*pc'; find $PNDSDK/usr/share/pkgconfig -name '*pc';find $PNDSDK/usr/bin -type f);do 
		if [ ! -d $i ];then
			sed -i "$SEDCMD" $i
		fi
	done

	for list in libc libpthread
	do
		sed -e 's#\/usr\/lib\/##g' $PNDSDK/usr/lib/$list.so > $PNDSDK/usr/lib/$list.so.new
		mv $PNDSDK/usr/lib/$list.so.new $PNDSDK/usr/lib/$list.so

		sed -e 's#\/lib\/##g' $PNDSDK/usr/lib/$list.so > $PNDSDK/usr/lib/$list.so.new
		mv $PNDSDK/usr/lib/$list.so.new $PNDSDK/usr/lib/$list.so
	done

	# add some symlinks for include files that were placed in subfolders
	# a recommended step from sebt3 to make sure that headers can be found
	cd $PNDSDK/usr/include
	for i in *-?.?
	do
		for j in $(ls -1 $i)
		do
			if [ ! -e "$j" ]
			then 
				ln -s $i/$j
			fi
		done
	done
	cd $CWD

	echo -e
	cecho "Applied known fixes, things should work now..." $green
}


create_extra_files()
{
	echo -e
	cecho "Creating additional files in $USEFUL_TOOLS_DIR" $green
	if [ ! -d "$USEFUL_TOOLS_DIR" ]
	then 
		cecho "Creating dir those additional files" $green
		mkdir -p $USEFUL_TOOLS_DIR
	fi
	
	cecho "Creating $USEFUL_TOOLS_DIR/PandoraToolchain.cmake" $green
	# Create default cmake toolchain file.
	cat > $USEFUL_TOOLS_DIR/PandoraToolchain.cmake <<_END_
# this one is important
SET(CMAKE_SYSTEM_NAME Linux)

# specify the cross compiler
SET(CMAKE_C_COMPILER   $PNDSDK/bin/arm-none-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER $PNDSDK/bin/arm-none-linux-gnueabi-g++)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  $PNDSDK/)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#usage sample with the sample case "wesnoth":
#current 1.10 setup:
#PATH=$PNDSDK/usr/bin:\$PATH CFLAGS="-DPANDORA -O3 -pipe -O3 -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -fPIC -ftree-vectorize -ffast-math " CXXFLAGS="-DPANDORA -O3 -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -fPIC -ftree-vectorize -ffast-math " cmake -DCMAKE_BUILD_TYPE=release -DENABLE_STRICT_COMPILATION=off -DCMAKE_TOOLCHAIN_FILE=$USEFUL_TOOLS_DIR/PandoraToolchain.cmake -DPKG_CONFIG_EXECUTABLE=$PNDSDK/bin/arm-none-linux-gnueabi-pkg-config -DSDL_CONFIG=$PNDSDK/usr/bin/sdl-config -DLIBINTL_INCLUDE_DIR=$PNDSDK/arm-none-linux-gnueabi/libc/usr/include/ -DPREFERENCES_DIR=wesnoth-1.10_userdata ../wesnoth-1.10.x
#
#current 1.11 setup:
#PATH=$PNDSDK/usr/bin:\$PATH CFLAGS="-DPANDORA -O3 -pipe -march=armv7-a -mcpu=cortex-a8 -mtune=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=softfp" CXXFLAGS="-DPANDORA -O3 -pipe -march=armv7-a -mcpu=cortex-a8 -mtune=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=softfp" cmake -DCMAKE_BUILD_TYPE=release -DENABLE_STRICT_COMPILATION=off -DCMAKE_TOOLCHAIN_FILE=$USEFUL_TOOLS_DIR/PandoraToolchain.cmake -DPKG_CONFIG_EXECUTABLE=$PNDSDK/bin/arm-none-linux-gnueabi-pkg-config -DSDL_CONFIG=$PNDSDK/usr/bin/sdl-config -DLIBINTL_INCLUDE_DIR=$PNDSDK/arm-none-linux-gnueabi/libc/usr/include/ -DPREFERENCES_DIR=wesnoth-1.11_userdata ../wesnoth-1.11.x
_END_
	
	cecho "Creating $USEFUL_TOOLS_DIR/pandora_configure.sh" $green
	# Create a script that makes using autotool based build systems easier.
	cat > $USEFUL_TOOLS_DIR/pandora_configure.sh <<_END_
#!/bin/bash
#
# This script is supposed to ease using the crosscompiler toolchain for the
# pandora with configure based build systems.
#
# Usage example:
# from inside the dir where you normally run a commandline like the following:
#    ./configure --prefix=\$HOME/pandora-dev/testcase --disable-strict-compilation
# run this instead to have everything setup for cross compilation:
#    $USEFUL_TOOLS_DIR/pandora_configure.sh --prefix=\$HOME/pandora-dev/testcase --disable-strict-compilation
# afterwards call 'make' and 'make install', as you are used to. For
# 'make install' you should define a '--prefix=' or you might get problems with
# your normal system.

PNDSDK=$PNDSDK
TARGET_SYS=$TARGET_SYS
CROSSTOOL="\$PNDSDK/bin/\$TARGET_SYS"

#on single long line with all the env vars that make sense to have...
PATH=\$PNDSDK/bin:\$PNDSDK/usr/bin:\$PATH \\
LIBTOOL_SYSROOT_PATH=\$PNDSDK \\
PKG_CONFIG_PATH=\$PNDSDK/usr/share/pkgconfig:\$PNDSDK/usr/lib/pkgconfig \\
PKG_CONFIG=\$PNDSDK/bin/arm-none-linux-gnueabi-pkg-config \\
CPATH="\$PNDSDK/usr/include:\$CPATH" \\
CFLAGS="-DPANDORA -O2 -pipe -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -fPIC -ftree-vectorize -ffast-math " \\
CPPFLAGS="-DPANDORA -O2 -pipe -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -fPIC -ftree-vectorize -ffast-math " \\
CXXFLAGS="-I\$PNDSDK/usr/include" \\
LDFLAGS="-L\$PNDSDK/usr/lib -Wl,-rpath,\$PNDSDK/usr/lib" \\
CXX=\$CROSSTOOL-g++ \\
CC=\$CROSSTOOL-gcc \\
AR=\$CROSSTOOL-ar \\
AS=\$CROSSTOOL-as \\
RANLIB=\$CROSSTOOL-ranlib \\
STRIP=\$CROSSTOOL-strip \\
HOST_CC=gcc \\
HOST_CXX=g++ \\
./configure --host=\$TARGET_SYS --with-gnu-ld \$*

if [ "\$?" -eq "0" ];
then
	echo -e
	echo -e
	echo "Running ./configure was completed. You can now run 'make'. In case you see any"
	echo "problems with your systems include dir being used when running 'make', please"
	echo "betry to export the changed CPATH value. This export will likely be problematic"
	echo "if you want to build non crosscompiled programs in the shell. General advice is"
	echo "to close the current shell after you are done, so that the changed CPATH"
	echo "environment variable is discarded. The export should look like this:"
	echo "export CPATH=\"\$PNDSDK/usr/include:\\\$CPATH\""
	echo "After exporting you can run 'make' just as you are used to."
fi
_END_
	chmod +x $USEFUL_TOOLS_DIR/pandora_configure.sh
	
	cecho "Creating $USEFUL_TOOLS_DIR/opkg-pandora.sh" $green
	# Create a script that makes running opkg easier.
	cat > $USEFUL_TOOLS_DIR/opkg-pandora.sh <<_END_
#!/bin/bash
#
# This script is supposed to ease using the opkg package manager with the sdk.
#
# Usage example:
# to get a list of installed packages you would normally run
#    opkg-cl list-installed
# With the sdk this is not possible because you have to provide many
# parameters. Instead use this script which will directly provide the params:
#    $USEFUL_TOOLS_DIR/opkg-pandora.sh list-installed
# This script does act as if it would be opkg. Be aware that with using the
# script you will likely create some problems and should rerun the script for
# fixing libs and various config files!

PNDSDK=$PNDSDK


\$PNDSDK/bin/opkg-cl --conf=\$PNDSDK/opkg.conf --offline-root=\$PNDSDK \$*
_END_
	chmod +x $USEFUL_TOOLS_DIR/opkg-pandora.sh
	
	cecho "Creating $USEFUL_TOOLS_DIR/fix-libs.sh" $green
	# Create a script that fixes lib entries.
	cat > $USEFUL_TOOLS_DIR/fix-libs.sh <<_END_
#!/bin/bash
#
# This script will fix your libs after running opkg tasks. You should always
# after using opkg to make sure that your libs are working.

PNDSDK_DIR=$PNDSDK_DIR
PNDSDK=$PNDSDK

SEDCMD="s#/usr/local/pandora#\$PNDSDK_DIR#g;s#prefix=\/.*#prefix=\$PNDSDK/usr#g;s#includedir=\/.*#includedir=\$PNDSDK/usr/include#g;s#libdir=\/.*#libdir='\$PNDSDK/usr/lib'#;s# /usr/lib/lib\([A-Z0-9a-z.-]*\).la# \$PNDSDK/usr/lib/lib\1.la#g;s# /usr/lib# \$PNDSDK/usr/lib#g;s#-L/usr/lib#-L\$PNDSDK/usr/lib#g;s#-R/usr/lib#-R\$PNDSDK/usr/lib#g;s#-I/usr/include#-I\$PNDSDK/usr/include#g;s#-isystem\/.*\b##g"

	echo "Fixing paths inside *la, *pc and files in usr/bin/"
	for i in \$(find \$PNDSDK/usr/lib -name '*la'; find \$PNDSDK/usr/lib/pkgconfig -name '*pc';find \$PNDSDK/usr/bin -type f);do 
		if [ ! -d \$i ];then
			sed -i "\$SEDCMD" \$i
		fi
	done

	for list in libc libpthread
	do
		sed -e 's#\/usr\/lib\/##g' \$PNDSDK/usr/lib/\$list.so > \$PNDSDK/usr/lib/\$list.so.new
		mv \$PNDSDK/usr/lib/\$list.so.new \$PNDSDK/usr/lib/\$list.so

		sed -e 's#\/lib\/##g' \$PNDSDK/usr/lib/\$list.so > \$PNDSDK/usr/lib/\$list.so.new
		mv \$PNDSDK/usr/lib/\$list.so.new \$PNDSDK/usr/lib/\$list.so
	done

	# add some symlinks for include files that were placed in subfolders
	# a recommended step from sebt3 to make sure that headers can be found
	cd \$PNDSDK/usr/include
	for i in *-?.?
	do
		for j in \$(ls -1 \$i)
		do
			if [ ! -e "\$j" ]
			then 
				ln -s \$i/\$j
			fi
		done
	done
_END_
	chmod +x $USEFUL_TOOLS_DIR/fix-libs.sh
	
	cecho "Creating $USEFUL_TOOLS_DIR/PXML_schema.xsd" $green
	# Create the PXML schema file
	cat > $USEFUL_TOOLS_DIR/PXML_schema.xsd <<_END_
<?xml version="1.0" encoding="utf-8"?>
<xs:schema targetNamespace="http://openpandora.org/namespaces/PXML" xmlns="http://openpandora.org/namespaces/PXML" xmlns:xs="http://www.w3.org/2001/XMLSchema" attributeFormDefault="unqualified" elementFormDefault="qualified">
	
	
	<!-- declare some simpleTypes for later usage -->
	
	<!-- Specify params allows with the 'x11' entry in exec -->
	<xs:simpleType name="x11Param">
		<xs:restriction base="xs:string">
			<xs:enumeration value="req" />
			<xs:enumeration value="stop" />
			<xs:enumeration value="ignore" />
		</xs:restriction>
	</xs:simpleType>
	
	<!-- Specify the valid documentation formats in the <info> block -->
	<xs:simpleType name="docType">
		<xs:restriction base="xs:string">
			<xs:enumeration value="text/html" />
			<xs:enumeration value="text/plain" />
		</xs:restriction>
	</xs:simpleType>
	
	<!-- Make sure that version numbers only consist of letters, numbers and + as well as - -->
	<xs:simpleType name="versionNumber">
		<xs:restriction base="xs:string">
			<xs:minLength value="1"/>
			<xs:pattern value="[a-zA-Z0-9+-]*" />
		</xs:restriction>
	</xs:simpleType>
	
	<!-- Specify what is valid as release type -->
	<xs:simpleType name="releaseType">
		<xs:restriction base="xs:string">
			<xs:enumeration value="alpha" />
			<xs:enumeration value="beta" />
			<xs:enumeration value="release" />
		</xs:restriction>
	</xs:simpleType>
	
	<!-- Specify what makes an email address "valid" -->
	<xs:simpleType name="emailAddress">
		<xs:restriction base="xs:string">
			<xs:pattern value="[^@]+@[^\.]+\..+"/>
		</xs:restriction>
	</xs:simpleType>
	
	<!-- some restrictions regarding file names that are eg not allowed/possible when using sd cards formated as fat32 -->
	<xs:simpleType name="dumbPath">
		<xs:restriction base="xs:normalizedString">
			<xs:pattern value="[^?>:]+" />
		</xs:restriction>
	</xs:simpleType>
	<xs:simpleType name="dumbFolderName">
		<xs:restriction base="xs:normalizedString">
			<xs:pattern value="[^?>:/]+" />
		</xs:restriction>
	</xs:simpleType>
	
	<!-- Specify lang codes -->
	<xs:simpleType name="isoLangcode">
		<xs:restriction base="xs:string">
			<xs:minLength value="2"/>
			<xs:pattern value="[a-zA-Z]{2,3}(_[a-zA-Z0-9]{2,3})*" />
		</xs:restriction>
	</xs:simpleType>
	<xs:simpleType name="isoLangcode_en_US">
		<xs:restriction base="xs:string">
			<xs:enumeration value="en_US" />
		</xs:restriction>
	</xs:simpleType>
	
	<!-- Definition of all allowed categories following the FDO specs -->
	<xs:simpleType name="fdoCategory">
		<xs:restriction base="xs:string">
			<xs:pattern value="AudioVideo|Audio|Video|Development|Education|Game|Graphics|Network|Office|Settings|System|Utility"/>
		</xs:restriction>
	</xs:simpleType>
	<!-- Definition of all allowed subcategories following the FDO specs (should be based upon the given main categories, but would significantly increase complexity of the schema) -->
	<xs:simpleType name="fdoSubCategory">
		<xs:restriction base="xs:string">
			<xs:pattern value="Building|Debugger|IDE|GUIDesigner|Profiling|RevisionControl|Translation|Calendar|ContactManagement|Database|Dictionary|Chart|Email|Finance|FlowChart|PDA|ProjectManagement|Presentation|Spreadsheet|WordProcessor|2DGraphics|VectorGraphics|RasterGraphics|3DGraphics|Scanning|OCR|Photography|Publishing|Viewer|TextTools|DesktopSettings|HardwareSettings|Printing|PackageManager|Dialup|InstantMessaging|Chat|IRCClient|FileTransfer|HamRadio|News|P2P|RemoteAccess|Telephony|TelephonyTools|VideoConference|WebBrowser|WebDevelopment|Midi|Mixer|Sequencer|Tuner|TV|AudioVideoEditing|Player|Recorder|DiscBurning|ActionGame|AdventureGame|ArcadeGame|BoardGame|BlocksGame|CardGame|KidsGame|LogicGame|RolePlaying|Simulation|SportsGame|StrategyGame|Art|Construction|Music|Languages|Science|ArtificialIntelligence|Astronomy|Biology|Chemistry|ComputerScience|DataVisualization|Economy|Electricity|Geography|Geology|Geoscience|History|ImageProcessing|Literature|Math|NumericalAnalysis|MedicalSoftware|Physics|Robotics|Sports|ParallelComputing|Amusement|Archiving|Compression|Electronics|Emulator|Engineering|FileTools|FileManager|TerminalEmulator|Filesystem|Monitor|Security|Accessibility|Calculator|Clock|TextEditor|Documentation|Core|KDE|GNOME|GTK|Qt|Motif|Java|ConsoleOnly"/>
		</xs:restriction>
	</xs:simpleType>
	
	<!-- Create some way to enforce entries to be nonempty -->
	<xs:simpleType name="nonempty_token">
		<xs:restriction base="xs:token">
			<xs:minLength value="1"/>
		</xs:restriction>
	</xs:simpleType>
		<xs:simpleType name="nonempty_string">
		<xs:restriction base="xs:string">
			<xs:minLength value="1"/>
		</xs:restriction>
	</xs:simpleType>
		<xs:simpleType name="nonempty_normalizedString">
		<xs:restriction base="xs:string">
			<xs:minLength value="1"/>
		</xs:restriction>
	</xs:simpleType>
	
	
	
	<!-- declare some complexTypes for later usage -->
	
	<!-- type used for file associations -->
	<xs:complexType name="association_data">
		<xs:attribute name="name" use="required" type="nonempty_normalizedString" />
		<xs:attribute name="filetype" use="required" type="nonempty_token" />
		<xs:attribute name="exec" use="required" type="nonempty_token" />
	</xs:complexType>
	
	<!-- type used for author info -->
	<xs:complexType name="author_data">
		<xs:attribute name="name" use="required" type="nonempty_normalizedString" />
		<xs:attribute name="website" use="optional" type="xs:anyURI" />
		<xs:attribute name="email" use="optional" type="emailAddress" />
	</xs:complexType>
	
	<!-- type used for version informations full entry) -->
	<xs:complexType name="app_version_info">
		<xs:attribute name="major" use="required" type="versionNumber" />
		<xs:attribute name="minor" use="required" type="versionNumber" />
		<xs:attribute name="release" use="required" type="versionNumber" />
		<xs:attribute name="build" use="required" type="versionNumber" />
		<xs:attribute name="type" use="optional" type="releaseType" />
	</xs:complexType>
	<!-- type used for OS version information (os version only) -->
	<xs:complexType name="os_version_info">
		<xs:attribute name="major" use="required" type="xs:nonNegativeInteger" />
		<xs:attribute name="minor" use="required" type="xs:nonNegativeInteger" />
		<xs:attribute name="release" use="required" type="xs:nonNegativeInteger" />
		<xs:attribute name="build" use="required" type="xs:nonNegativeInteger" />
	</xs:complexType>
	
	<!-- type used for exec entries -->
	<xs:complexType name="exec_params">
		<xs:attribute name="command" use="required" type="nonempty_token" />
		<xs:attribute name="arguments" use="optional" type="nonempty_token" />
		<xs:attribute name="background" use="optional" type="xs:boolean" />
		<xs:attribute name="startdir" use="optional" type="dumbPath" />
		<xs:attribute name="standalone" use="optional" type="xs:boolean" />
		<xs:attribute name="x11" use="optional" type="x11Param" />
	</xs:complexType>
	
	<!-- type used for tiles or descriptions, once in 'normal' version, once enforcing usage of en_US -->
	<xs:complexType name="title_or_description">
		<xs:simpleContent>
			<xs:extension base="nonempty_string">
				<xs:attribute name="lang" use="required" type="isoLangcode" />
			</xs:extension>
		</xs:simpleContent>
	</xs:complexType>
	<xs:complexType name="title_or_description_enUS">
		<xs:simpleContent>
			<xs:extension base="nonempty_string">
				<xs:attribute name="lang" use="required" type="isoLangcode_en_US" />
			</xs:extension>
		</xs:simpleContent>
	</xs:complexType>
	
	<!-- type used for referencing images -->
	<xs:complexType name="image_entry">
		<xs:attribute name="src" use="required" type="dumbPath" />
	</xs:complexType>
	
	<!-- type for referencing manuals/readme docs -->
	<xs:complexType name="information_entry">
		<xs:attribute name="name" use="required" type="nonempty_normalizedString" />
		<xs:attribute name="type" use="required" type="docType" />
		<xs:attribute name="src" use="required" type="dumbPath" />
	</xs:complexType>
	
	<!-- type used for the license information -->
	<xs:complexType name="license_info">
		<xs:attribute name="name" use="required" type="nonempty_normalizedString" />
		<xs:attribute name="url" use="optional" type="xs:anyURI" />
		<xs:attribute name="sourcecodeurl" use="optional" type="xs:anyURI" />
	</xs:complexType>
	
	
	
	<!-- Combine the symple and complex types into the "real" PXML specification -->
	
	<xs:element name="PXML">
		<xs:complexType>
			<xs:sequence>
				<!-- specify the <package> tag with info about the complete package, information providable:
					author
					version
					title(s)
					description(s)
					icon
					-->
				<xs:element name="package" minOccurs="1" maxOccurs="1">
					<xs:complexType>
						<xs:all>
							<!--Author info-->
							<xs:element name="author" type="author_data" minOccurs="1" />
							<!--App version info-->
							<xs:element name="version" type="app_version_info" minOccurs="1" />
							<!--Title-->
							<xs:element name="titles" minOccurs="1">
								<xs:complexType>
									<xs:sequence>
										<xs:element name="title" type="title_or_description_enUS" minOccurs="1" maxOccurs="1" />
										<xs:element name="title" type="title_or_description" minOccurs="0" maxOccurs="unbounded" />
									</xs:sequence>
								</xs:complexType>
							</xs:element>
							<!--Description-->
							<xs:element name="descriptions" minOccurs="0">
								<xs:complexType>
									<xs:sequence>
										<xs:element name="title" type="title_or_description_enUS" minOccurs="0" maxOccurs="1" />
										<xs:element name="description" type="title_or_description" minOccurs="0" maxOccurs="unbounded" />
									</xs:sequence>
								</xs:complexType>
							</xs:element>
							<!--Icon-->
							<xs:element name="icon" type="image_entry" minOccurs="0" />
						</xs:all>
						<!--Package ID-->
						<xs:attribute name="id" use="required" type="dumbFolderName" />
					</xs:complexType>
				</xs:element>
				<!-- specify the <application> tag with info about a single program
					executable call
					author
					version (of the application)
					osversion (min OS version supported)
					title(s) (allowing compatibility to <HF6, too!)
					description(s) (allowing compatibility to <HF6, too!)
					icon
					license
					preview pictures
					info/manual/readme entry
					categories
					associations to file types
					clockspeed
					-->
				<xs:element name="application" minOccurs="1" maxOccurs="unbounded">
					<xs:complexType>
						<xs:all>
							<!--Execution params -->
							<xs:element name="exec" type="exec_params" minOccurs="1" />
							<!--Author info-->
							<xs:element name="author" type="author_data" minOccurs="1" />
							<!--App version info-->
							<xs:element name="version" type="app_version_info" minOccurs="1" />
							<!--OS Version info-->
							<xs:element name="osversion" type="os_version_info" minOccurs="0" />
							<!--Title-->
							<!-- via <titles> element, used for HF6+ -->
							<xs:element name="titles" minOccurs="1">
								<xs:complexType>
									<xs:sequence>
										<xs:element name="title" type="title_or_description_enUS" minOccurs="1" maxOccurs="1" />
										<xs:element name="title" type="title_or_description" minOccurs="0" maxOccurs="unbounded" />
									</xs:sequence>
								</xs:complexType>
							</xs:element>
							<!--Title-->
							<!-- via <title> element, only one for en_US allowed, meant for backwards compatibility with libpnd from <HF6 -->
							<xs:element name="title" type="title_or_description_enUS" minOccurs="0" />
							<!--Description-->
							<!-- via <descriptions> element, used for HF6+ -->
							<xs:element name="descriptions" minOccurs="0">
								<xs:complexType>
									<xs:sequence>
										<xs:element name="description" type="title_or_description_enUS" minOccurs="1" maxOccurs="1" />
										<xs:element name="description" type="title_or_description" minOccurs="0" maxOccurs="unbounded" />
									</xs:sequence>
								</xs:complexType>
							</xs:element>
							<!--Description-->
							<!-- via <description> element, only one for en_US allowed, meant for backwards compatibility with libpnd from <HF6 -->
							<xs:element name="description" type="title_or_description_enUS" minOccurs="0" />
							<!--Icon-->
							<xs:element name="icon" type="image_entry" minOccurs="0" />
							<!--License-->
							<xs:element name="licenses" minOccurs="1">
								<xs:complexType>
									<xs:sequence>
										<xs:element name="license" type="license_info" minOccurs="1" maxOccurs="unbounded" />
									</xs:sequence>
								</xs:complexType>
							</xs:element>
							<!--Preview pics-->
							<xs:element name="previewpics" minOccurs="0">
								<xs:complexType>
									<xs:sequence>
										<xs:element name="pic" type="image_entry" minOccurs="0" maxOccurs="unbounded" />
									</xs:sequence>
								</xs:complexType>
							</xs:element>
							<!--Info (aka manual or readme entry)-->
							<xs:element name="info" type="information_entry" minOccurs="0" />
							<!--Categories-->
							<xs:element name="categories" minOccurs="1">
								<xs:complexType>
									<xs:sequence>
										<xs:element name="category" minOccurs="1" maxOccurs="unbounded">
											<xs:complexType>
												<xs:sequence>
													<xs:element name="subcategory" minOccurs="0" maxOccurs="unbounded">
														<xs:complexType>
															<xs:attribute name="name" type="fdoSubCategory" />
														</xs:complexType>
													</xs:element>
												</xs:sequence>
												<xs:attribute name="name" use="required" type="fdoCategory" />
											</xs:complexType>
										</xs:element>
									</xs:sequence>
								</xs:complexType>
							</xs:element>
							<!--Associations-->
							<xs:element name="associations" minOccurs="0">
								<xs:complexType>
									<xs:sequence>
										<xs:element name="association" type="association_data" maxOccurs="unbounded" />
									</xs:sequence>
								</xs:complexType>
							</xs:element>
							<!--Clockspeed-->
							<xs:element name="clockspeed" minOccurs="0">
								<xs:complexType>
									<xs:attribute name="frequency" use="required" type="xs:positiveInteger" />
								</xs:complexType>
							</xs:element>
						</xs:all>
						<!--AppID-->
						<xs:attribute name="id" use="required" type="dumbFolderName" />
						<xs:attribute name="appdata" use="optional" type="dumbFolderName" />
					</xs:complexType>
				</xs:element>
			</xs:sequence>
		</xs:complexType>
	</xs:element>
</xs:schema>
_END_

	cecho "Creating $USEFUL_TOOLS_DIR/pnd_make.sh" $green
	# Create a script for creating pnd packages.
	cat > $USEFUL_TOOLS_DIR/pnd_make.sh <<_END_
#!/bin/bash
#
# pnd_make.sh
#
# This script is meant to ease generation of a pnd file. Please consult the output
# when running --help for a list of available parameters and an explaination of
# those.
#
# Required tools when running the script:
# bash
# echo, cat, mv, rm
# mkisofs or mksquashfs (the latter when using the -c param!)
# xmllint (optional, only for validation of the PXML against the schema)


PXML_schema=\$(dirname \${0})/PXML_schema.xsd
GENPXML_PATH=\$(dirname \${0})/genpxml.sh

# useful functions ...
black='\E[30m'
red='\E[31m'
green='\E[32m'
yellow='\E[33m'
blue='\E[34m'
magenta='\E[35m'
cyan='\E[36m'
white='\E[37m'

check_for_tool()
{
	which \$1 &> /dev/null
	if [ "\$?" -ne "0" ];
	then
		cecho "ERROR: Could not find the program '\$1'. Please make sure
that it is available in your PATH since it is required to complete your request." \$red
		exit 1
	fi
}

cecho ()	# Color-echo. Argument \$1 = message, Argument \$2 = color
{
	local default_msg="No message passed."   # Doesn't really need to be a local variable.
	message=\${1:-\$default_msg}               # Defaults to default message.
	color=\${2:-\$black}                       # Defaults to black, if not specified.
	echo -e "\$color\$message"
	tput sgr0                                # Reset to normal.
	return
} 


print_help()
{
	cat << EOSTREAM
pnd_make.sh - A script to package "something" into a PND.

Usage:
  \$(basename \${0}) {--directory|-d} <folder> {--pndname|-p} <file> [{--compress-squashfs|-c}]
                   [{--genpxml} <file>] [{--icon|-i} <file>] [{--pxml|-x} <file>]
                   [{--schema|-s} <file>] [{--help|-h}]


Switches:
  --compress-squashfs / -c  Define whether or not the pnd should be compressed using
                            squashfs. If this parameter is selected, a compressed pnd
                            will be created.

  --directory / -d          Sets the folder that is to be used for the resulting pnd
                            to <folder>. This option is mandatory for the script to
                            function correctly.

  --genpxml                 Sets the script used for generating a PXML file (if none
                            is available already) to <file>. Please make sure to either
                            provide a full path or prefix a script in the current folder
                            with './' so that the script can actually be executed. If
                            this variable is not specified, \$GENPXML_PATH
                            will be used.

  --help / -h               Displays this help text.

  --icon / -i               Sets the icon that will be appended in the pnd to <file>.

  --pndname / -p            Sets the output filename of the resulting pnd to <file>.
                            This option is mandatory for the script to function
                            correctly.

  --pxml / -x               Sets the PXML file that is to be used to <file>. If you
                            neither provide a PXML file or set this entry to 'guess',
                            an existing 'PXML.xml' in your selected '--directory'
                            will be used, or the script \$GENPXML_PATH
                            will be called to try to generate a basic PXML file for you.

  --schema / -s             Sets the schema file, that is to be used for validation,
                            to <file. If this is not defined, the script will try to
                            use the file '\$PXML_schema'. If this fails,
                            a warning is issued.

If you select the option to create a compressed squashfs, a version >=4.0 of squashfs
is required to be available in your PATH.
EOSTREAM
}


# Parse command line parameters
while [ "\${1}" != "" ]; do
	if [ "\${1}" = "--compress-squashfs" ] || [ "\${1}" = "-c" ];
	then
		SQUASH=1
		shift 1
	elif [ "\${1}" = "--directory" ] || [ "\${1}" = "-d" ];
	then
		FOLDER=\$2
		shift 2
	elif [ "\${1}" = "--genpxml" ];
	then
		GENPXML_PATH=\$2
		shift 2
	elif [ "\${1}" = "--help" ] || [ "\${1}" = "-h" ];
	then
		print_help
		exit 0
	elif [ "\${1}" = "--icon" ] || [ "\${1}" = "-i" ];
	then
		ICON=\$2
		shift 2
	elif [ "\${1}" = "--pndname" ] || [ "\${1}" = "-p" ];
	then
		PNDNAME=\$2
		shift 2
	elif [ "\${1}" = "--pxml" ] || [ "\${1}" = "-x" ];
	then
		PXML=\$2
		shift 2
	elif [ "\${1}" = "--schema" ] || [ "\${1}" = "-f" ]
	then
		PXML_schema=\$2
		shift 2
	else
		cecho "ERROR: '\$1' is not a known argument. Printing --help and aborting." \$red
		print_help
		exit 1
	fi
done


# Generate a PXML if the param is set to Guess or it is empty.
if [ ! \$PXML ] || [ \$PXML = "guess" ] && [ \$PNDNAME ] && [ \$FOLDER ];
then
	if [ -f \$FOLDER/PXML.xml ]; # use the already existing PXML.xml file if there is one...
	then
		PXML=\$FOLDER/PXML.xml
		PXML_ALREADY_EXISTING="true"
	else
		if [ -f \$GENPXML_PATH ];
		then
			\$GENPXML_PATH --src \$FOLDER --dest \$FOLDER --author \$USER
			if [ -f \$FOLDER/PXML.xml ];
			then
				PXML_GENERATED="true"
			else
				cecho "ERROR: Generating a PXML file using '\$GENPXML_PATH' failed.
Please generate a PXML file manually." \$red
				exit 1
			fi
		else
			cecho "ERROR: Could not find '\$GENPXML_PATH' for generating a PXML file." \$red
			exit 1
		fi
	fi
fi


# Probe if required variables were set
echo -e
cecho "Checking if all required variables were set." \$green
if [ ! \$PNDNAME ] || [ ! \$FOLDER ] || [ ! \$PXML ];
then
	echo -e
	cecho "ERROR: Not all required options were set! Please see the --help information below." \$red
	echo -e
	print_help
	exit 1
else
	echo "PNDNAME set to '\$PNDNAME'."
fi
# Check if the selected folder actually exists
if [ ! -d \$FOLDER ];
then
	echo -e
	cecho "ERROR: '\$FOLDER' doesn't exist or is not a folder." \$red
	exit 1
else
	echo "FOLDER set to '\$FOLDER'."
fi
# Check if the selected PXML file actually exists
if [ ! -f \$PXML ];
then
	echo -e
	cecho "ERROR: '\$PXML' doesn't exist or is not a file." \$red
	exit 1
else
	if [ \$PXML_ALREADY_EXISTING ];
	then
		echo "You have not explicitly specified a PXML to use, but an existing file was
found. Will be using this one."
	elif [ \$PXML_GENERATED ];
	then
		echo "A PXML file was generated for you using '\$GENPXML_PATH'. This file will
not be removed at the end of this script because you might want to review it, adjust
single entries and rerun the script to generate a pnd with a PXML file with all the
information you want to have listed."
	fi
	echo "PXML set to '\$PXML'."
fi

# Print the other variables:
if [ \$ICON ];
then
	if [ ! -f \$ICON ]
	then
		cecho "WARNING: '\$ICON' doesn't exist, will not append the selected icon to the pnd." \$red
	else
		echo "ICON set to '\$ICON'."
		USE_ICON="true"
	fi
fi
if [ \$SQUASH ];
then
	echo "Will use a squashfs for '\$PNDNAME'."
fi


# Validate the PXML file (if xmllint is available)
# Errors and problems in this section will be shown but are not fatal.
echo -e
cecho "Trying to validate '\$PXML' now. Will be using '\$PXML_schema' to do so." \$green
which xmllint &> /dev/null
if [ "\$?" -ne "0" ];
then
	VALIDATED=false
	cecho "WARNING: Could not find 'xmllint'. Validity check of '\$PXML' is not possible!" \$red
else
	if [ ! -f "\$PXML_schema" ];
	then
		VALIDATED=false
		cecho "WARNING: Could not find '\$PXML_schema'. If you want to validate your
PXML file please make sure to provide a schema using the --schema option." \$red
	else
		xmllint --noout --schema \$PXML_schema \$PXML
		if [ "\$?" -ne "0" ]; then VALIDATED=false; else VALIDATED=true; fi
	fi
fi
# Print some message at the end about the validation in case the user missed the output above
if [ \$VALIDATED = "false" ]
then
	cecho "WARNING: Could not successfully validate '\$PXML'. Please check the output
above. This does not mean that your pnd will be broken. Either you are not following the strict
syntax required for validation or you don't have all files/programs required for validating." \$red
else
	cecho "Your file '\$PXML' was validated successfully. The resulting pnd should
work nicely with libpnd." \$green
fi


# Make iso from folder
echo -e
cecho "Creating an iso file based on '\$FOLDER'." \$green
if [ \$SQUASH ];
then
	check_for_tool mksquashfs
	if [ \$(mksquashfs -version | awk 'BEGIN{r=0} \$3>=4{r=1} END{print r}') -eq 0 ];
	then
		cecho "ERROR: Your squashfs version is older then version 4, please upgrade to 4.0 or later" \$red
		exit 1
	fi
	mksquashfs \$FOLDER \$PNDNAME.iso -nopad -no-recovery
else
	check_for_tool mkisofs
	mkisofs -o \$PNDNAME.iso -R \$FOLDER
fi

# Check that the iso file was actually created before continuing
if [ ! -f \$PNDNAME.iso ];
then
	cecho "ERROR: The temporary file '\$PNDNAME.iso' could not be created.
Please check the output above for any errors and retry after fixing them. Aborting." \$red
	exit 1
fi


# Append pxml to iso
echo -e
cecho "Appending '\$PXML' to the created iso file." \$green
cat \$PNDNAME.iso \$PXML > \$PNDNAME
rm \$PNDNAME.iso #cleanup


# Append icon if specified and available
if [ \$USE_ICON ];
then
	echo -e
	cecho "Appending the icon '\$ICON' to the pnd." \$green
	mv \$PNDNAME \$PNDNAME.tmp
	cat \$PNDNAME.tmp \$ICON > \$PNDNAME # append icon
	rm \$PNDNAME.tmp #cleanup
fi


# Final message
echo -e
if [ -f \$PNDNAME ];
then
	cecho "Successfully finished creating the pnd '\$PNDNAME'." \$green
else
	cecho "There seems to have been a problem and '\$PNDNAME' was not created. Please check
the output above for any error messages. A possible cause for this is that there was
not enough space available." \$red
	exit 1
fi


#if [ \$PXML = "guess" ];then rm \$FOLDER/PXML.xml; fi #cleanup
_END_
	chmod +x $USEFUL_TOOLS_DIR/pnd_make.sh
	
	cecho "Creating $USEFUL_TOOLS_DIR/genpxml.sh" $green
	# Create a script that generates a basic PXML.xml file.
	cat > $USEFUL_TOOLS_DIR/genpxml.sh <<_END_
#!/bin/bash

help(){
cat >&2 <<ENDHELP
\$0 [args] :
-s|--src <src dir>      : Define source directory (default: \$SRCDIR)
-n|--name <name>        : Define the pnd base name (default: \$PND_NAME)
-d|--dest <dest dir>    : Define destination directory (default: \$DESTDIR)
-a|--author <name>      : programmers names (default: \$AUTHOR)
-v|--version <version>  : Define the version (default: \$VERSION)
-w|--website <url>      : Define the url (default: \$WEBSITE)
-b|--build <build id>   : Define the build number (default: \$BUILD)
-f|--force              : overide PXML.xml file if found
-h|--help               : show this screen
ENDHELP
}

DEBUG(){
	echo \$*>&2
}

buildApplicationList(){
	#output "<appname> [<desktopfile>]" lines
	cd \$DESTDIR
	DESTLST=\$(find \$SRCDIR -name "*desktop";find \$DESTDIR -name "*desktop")
	if [ ! -z "\$DESTLST" ];then
		{
		for d in \$DESTLST;do
			EXE=\$(awk -F= '/Exec/{print \$2}'<\$d|awk '{print \$1}'|head -1)
			if [[ "x\$EXE" != "x" ]];then
				BIN=\$(find . -executable -type f -name \$(basename \$EXE)|head -1)
				echo \${BIN:-\$EXE} \$d
			fi
		done
		}|sort|awk 'BEGIN{P=""}\$1!=P{print}{P=\$1}'
	else
		find . -executable -type f \! -name "*sh"
	fi
}

getPATH(){
	cd \$DESTDIR
	L=\$(find . -type d -name bin)
	echo \$L|sed "s#\./#\$(pwd)/#g;s# #:#g"
}
getLIBPATH(){
	cd \$DESTDIR
	L=\$(find . -type d -name lib)
	echo \$L|sed "s#\./#\$(pwd)/#g;s# #:#g"
}

genLaunchScript() {
	S="\$DESTDIR/scripts/\$(basename \$1).sh"
	if [ -e \$S ];then
		mv \$S \${S}.old
	fi
	cat>\$S<<ENDSCRIPT
#!/bin/sh
export PATH="\$(getPATH):\\${PATH:-"/usr/bin:/bin:/usr/local/bin"}"
export LD_LIBRARY_PATH="\$(getLIBPATH):\\${LD_LIBRARY_PATH:-"/usr/lib:/lib"}"
export HOME="/mnt/utmp/\$PND_NAME" XDG_CONFIG_HOME="/mnt/utmp/\$PND_NAME"

if [ -d /mnt/utmp/\$PND_NAME/share ];then
	export XDG_DATA_DIRS=/mnt/utmp/\$PND_NAME/share:\\$XDG_DATA_DIRS:/usr/share
fi
export SDL_AUDIODRIVER="alsa"
cd \\$HOME
[ -e "\\$HOME/scripts/pre_script.sh" ] && . \\$HOME/scripts/pre_script.sh
if [ -e "\\$HOME/scripts/post_script.sh" ];then
	\$1 \\$*
	. \\$HOME/scripts/post_script.sh
else
	exec \$1 \\$*
fi
ENDSCRIPT
	chmod 755 \$S
}

desktop2application(){ #generate PXML application info out of a desktop file passed as parameter
	DESK=\$1
	if [ ! -z "\$DESK" ];then
		DVERSION=\$(awk -F= '\$1=="Version"{print \$2}'<\$1)
		if [ -z "\$DVERSION" ] || [[ "\$DVERSION" = "1.0" ]];then
			DVERSION=\$VERSION
		fi
	else
		DVERSION=\$VERSION
	fi
	MAJOR=\$(echo \$DVERSION|awk -F. '{print \$1}')
	MINOR=\$(echo \$DVERSION|awk -F. '{print \$2}')
	REL=\$(echo \$DVERSION|awk -F. '{print \$3}')
	MINOR=\${MINOR:-"0"}
	REL=\${REL:-"0"}

	# Exec --------------------------
	echo "    <exec command=\"scripts/\${BASEB}.sh\"/>"

	# title -------------------------
	if [ ! -z "\$DESK" ];then
		#awk -F= '(\$1~/^Name/)||\$1=="Name"{print \$1" "\$2}'<\$1|while read DN DNV;do
		awk -F= '(\$1~/^Name/&&/en_US/)||\$1=="Name"{print \$1" "\$2}'<\$1|while read DN DNV;do
			l=\$(echo \$DN|sed 's/Name//;s/\[//;s/\]//')
			echo "    <title lang=\"\${l:-"en_US"}\">\$DNV</title>"
		done
	else
		echo "    <title lang=\"en_US\">\$PND_NAME</title>"
	fi
	echo

	# Author ------------------------
	echo "    <author name=\"\$AUTHOR\" website=\"\$WEBSITE\"/>"

	# Version -----------------------
	echo "    <version major=\"\$MAJOR\" minor=\"\$MINOR\" release=\"\$REL\" build=\"\$BUILD\"/>	      <!--This programs version-->"

	# OS Version --------------------
	echo "    <osversion major=\"1\" minor=\"0\" release=\"0\" build=\"0\"/>		<!--The minimum OS version required-->"
	echo

	# Description -------------------
	if [ ! -z "\$DESK" ];then
		#awk -F= '(\$1~/^Comment/)||\$1=="Comment"{print \$1" "\$2}'<\$1|while read DN DNV;do
		awk -F= '(\$1~/^Comment/&&/en_US/)||\$1=="Comment"{print \$1" "\$2}'<\$1|while read DN DNV;do
			l=\$(echo \$DN|sed 's/Comment//;s/\[//;s/\]//')
			echo "    <description lang=\"\${l:-"en_US"}\">\$DNV</description>"
		done
	else
		echo "    <description lang=\"en_US\">Automatically generated description from \$(pwd) for PND=\$PND_NAME</description>"
	fi
	echo

	# Icon --------------------------
	ICON=""
	if [ ! -z "\$DESK" ];then
		ICON=\$(awk -F= '\$1~/^Icon/{print \$2}'<\$1)
	fi
	if [ ! -z "\$ICON" ];then
		F=\$(find \$DESTDIR -name \${ICON}.png|head -1)
		if [ -z "\$F" ];then
			F=\$DESTDIR/icon.png
		fi
		echo "    <icon src="'"'\$F'"/>'
		if [ ! -e "\$DESTDIR/icon.png" ] && [ ! -z "\$(find \$DESTDIR -name \${ICON}.png|head -1)" ];then
			cp \$(find \$DESTDIR -name \${ICON}.png|head -1) \$DESTDIR/icon.png
		fi
	elif [ -e "\$DESTDIR/icon.png" ];then
		echo "    <icon src=\"\$DESTDIR/icon.png\"/>"
	else
		echo "    <!--<icon src=\"path/to/icon.pnd\"/>-->"
	fi
	echo

	# Preview pics ------------------
	if [ ! -z "\$(find \$DESTDIR/previews -name "\$BASEB*")" ];then
		echo "    <previewpics>"
		for i in \$(find \$DESTDIR/previews -name "\$BASEB*");do
			echo "      <pic src=\"previews/\$(basename \$i)\"/>"
		done
		echo "    </previewpics>"
	else
		cat <<ENDASSO
<!--
    <previewpics>
      <pic src="previews/\${BASEB}.png"/>
    </previewpics>
-->
ENDASSO
	fi
	echo

	# Documentation -----------------
	HTML=\$(find \$DESTDIR -type d -name index.html|head -1)
	DOC=\$DESTDIR/readme.txt
	if [ ! -z "\$HTML" ];then
		for i in \$(find \$DESTDIR -type d -name index.html);do
			echo "    <info name=\"\${BASEB} documentation\" type=\"text/html\" src=\"\$i\"/>"
		done
	elif [ -e "\$DOC" ];then
		echo "    <info name=\"\${BASEB} documentation\" type=\"text/plain\" src=\"\$DOC\"/>"
	else
		echo "    <!--<info name=\"\${BASEB} documentation\" type=\"text/plain\" src=\"\$DOC\"/>-->"
	fi

	# Categories --------------------
	cat <<ENDCATEGORIES

    <categories>
      <!-- http://standards.freedesktop.org/menu-spec/latest/apa.html -->
ENDCATEGORIES

	if [ ! -z "\$DESK" ];then
		DCAT=\$(awk -F= '\$1=="Categories"{print \$2}'<\$1)
	else
		DCAT=""
	fi
	CATCNT=\$((\$(echo \$DCAT|sed "s/;/ /g"|wc -w) / 2))
	if [ \$CATCNT -gt 0 ];then
		for i in \$(seq 1 \$CATCNT);do
			DCATMAJ=\$(echo \$DCAT|awk -F\; "{print \\$\$((\$i*2-1))}")
			DCATMIN=\$(echo \$DCAT|awk -F\; "{print \\$\$((\$i*2))}")
			cat <<ENDCATEGORIES
      <category name="\$DCATMAJ">
	<subcategory name="\$DCATMIN"/>
      </category>
ENDCATEGORIES
		done
	else
		cat <<ENDCATEGORIES
      <category name="Game">
	<subcategory name="ArcadeGame"/>
      </category>
ENDCATEGORIES
	fi
	cat <<ENDCATEGORIES
    </categories>

ENDCATEGORIES

	# Associations ------------------
	cat <<ENDASSO
<!--
    <associations>
      <association name="Deinterlaced Bitmap Image" filetype="image/bmp" exec="-f %s"/>
      <association name="Style sheet system crasher" filetype="text/css" exec="-f %s"/>
    </associations>
-->
ENDASSO

	# ClockSpeed --------------------
	echo "    <!--<clockspeed frequency=\"600\"/>-->"
}

genPxml(){
	# output the PXML.xml file
	if [ -e \$DESTDIR/PXML.xml ];then
		mv \$DESTDIR/PXML.xml \$DESTDIR/PXML.xml.old
	fi
	cat >\$DESTDIR/PXML.xml <<ENDHEAD
<?xml version="1.0" encoding="UTF-8"?>
<PXML xmlns="http://openpandora.org/namespaces/PXML">
<!-- please see http://pandorawiki.org/PXML_specification for more information before editing, and remember the order does matter -->

ENDHEAD
	if [ ! -d \$DESTDIR/previews ];then
		mkdir -p \$DESTDIR/previews
	fi
	if [ ! -d \$DESTDIR/scripts ];then
		mkdir \$DESTDIR/scripts
	fi
	buildApplicationList|while read BIN DESK;do
		BASEB=\$(basename \$BIN)
		genLaunchScript \$BIN
		cat >>\$DESTDIR/PXML.xml <<ENDAPP
  <application id="\$PND_NAME-\$(basename \$BIN)-\$RND" appdata="\$PND_NAME">
ENDAPP
		desktop2application \$DESK >>\$DESTDIR/PXML.xml
		cat >>\$DESTDIR/PXML.xml <<ENDINFO
  </application>

ENDINFO
	done
	echo "</PXML>" >>\$DESTDIR/PXML.xml
}


#####################
### Script main :
##

FORCE=0
BUILD=1
AUTHOR=sebt3
WEBSITE=\${WEBSITE:-"http://www..openpandora.org"}
SRCDIR=\${SRCDIR:-\$(pwd)}
PND_NAME=\$PRJ
PND_NAME=\${PND_NAME:-\$(basename \$SRCDIR|awk -F- '{print \$1}')}
VERSION=\${VERSION:-\$(basename \$SRCDIR|awk -F- '{print \$2}')}
DESTDIR=\${DESTDIR:-"/mnt/utmp/\$PND_NAME"}
RND=\$RANDOM
# Parse arguments
while [ \$# -gt 0 ];do
	case \$1 in
	-s|--src)       SRCDIR=\$2;shift;;
	-d|--dest)      DESTDIR=\$2;shift;;
	-b|--build)     BUILD=\$2;shift;;
	-a|--author)    AUTHOR=\$2;shift;;
	-n|--name)      PND_NAME=\$2;shift;;
	-v|--version)   VERSION=\$2;shift;;
	-w|--website)   WEBSITE=\$2;shift;;
	-f|--force)     FORCE=1;;
	-h|--help)      help;exit 1;;
	*)	      echo "'\$1' unknown argument">&2;help;exit 2;;
	esac
	shift;
done

# Validate arguments
if [ ! -d \$SRCDIR ];then
	echo "\$SRCDIR don't exist" >&2
	help
	exit 3
fi
if [ ! -d \$DESTDIR ] && [ \$FORCE -eq 0 ];then
	echo "\$DESTDIR don't exist" >&2
	help
	exit 4
elif [ ! -d \$DESTDIR ];then
	mkdir -p \$DESTDIR
	if [ \$? -ne 0 ];then
		echo "\$DESTDIR don\'t exist and cannot be created" >&2
		help
		exit 5
	fi
fi
if [ \$(buildApplicationList|wc -l) -le 0 ];then
	echo "No applications found">&2
	help
	exit 6
fi
if [ -e \$DESTDIR/PXML.xml ] && [ \$FORCE -eq 0 ];then
	echo "PXML file exist and force disabled." >&2
	help 
	exit 7
fi
genPxml

_END_
	chmod +x $USEFUL_TOOLS_DIR/genpxml.sh
	
	mkdir -p $USEFUL_TOOLS_DIR/qmake_linux-pandora-g++
	# Some binaries from the users $PATH are used, those have a suffix on some
	# Distributions. If no version is found, use the -qt4 verison. Lets find
	# out which version to use...
	which moc &> /dev/null
	if [ "$?" -eq "0" ];
	then
		MOC_NAME=moc
	else
		MOC_NAME=moc-qt4
	fi
	which uic &> /dev/null
	if [ "$?" -eq "0" ];
	then
		UIC_NAME=uic
	else
		UIC_NAME=uic-qt4
	fi
	cecho "Creating $USEFUL_TOOLS_DIR/qmake_linux-pandora-g++/qmake.conf" $green
	# Create a file that allows using qmake with the SDK
	cat > $USEFUL_TOOLS_DIR/qmake_linux-pandora-g++/qmake.conf <<_END_
MAKEFILE_GENERATOR      = UNIX
CONFIG                 += nostrip
CONFIG                 += qt warn_on release incremental link_prl
DEFINES += QT_DEFAULT_TEXTURE_GLYPH_CACHE_WIDTH=1024
DEFINES += QT_GL_NO_SCISSOR_TEST
DEFINES += NO_DEBUG_ON_CONSOLE
QT_INSTALL_BINS		= /usr/bin
QMAKE_AR              = $PNDSDK/bin/arm-none-linux-gnueabi-ar cqs
QMAKE_CC		= $PNDSDK/bin/arm-none-linux-gnueabi-gcc
QMAKE_CFLAGS_DEPS	+= -M
#QMAKE_CFLAGS_HIDESYMS   += -fvisibility=hidden
QMAKE_CFLAGS		+= -pipe
QMAKE_CFLAGS_RELEASE    += -fno-omit-frame-pointer -fno-optimize-sibling-calls
QMAKE_CFLAGS_RELEASE	+= -DPANDORA -O3 pipe -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -fPIC -ftree-vectorize -ffast-math 
QMAKE_CFLAGS_SHLIB	+= -fPIC
QMAKE_CFLAGS_STATIC_LIB	+= -fPIC
QMAKE_CFLAGS_THREAD	+= -D_REENTRANT
QMAKE_CFLAGS_WARN_OFF	+= -w
QMAKE_CFLAGS_WARN_ON	+= -Wall -W
QMAKE_CFLAGS_YACC	+= -Wno-unused -Wno-parentheses
QMAKE_CHK_DIR_EXISTS  = test -d
QMAKE_COPY            = cp -f
QMAKE_COPY_DIR        = \$(COPY) -r
QMAKE_COPY_FILE       = \$(COPY)
QMAKE_CXXFLAGS_DEBUG	+= \$\$QMAKE_CFLAGS_DEBUG
QMAKE_CXXFLAGS_DEPS	+= \$\$QMAKE_CFLAGS_DEPS
#QMAKE_CXXFLAGS_HIDESYMS += \$\$QMAKE_CFLAGS_HIDESYMS -fvisibility-inlines-hidden
QMAKE_CXXFLAGS_HIDESYMS += \$\$QMAKE_CFLAGS_HIDESYMS
QMAKE_CXXFLAGS		+= \$\$QMAKE_CFLAGS
QMAKE_CXXFLAGS_RELEASE  += -g -fno-omit-frame-pointer -fno-optimize-sibling-calls
QMAKE_CXXFLAGS_RELEASE	+= \$\$QMAKE_CFLAGS_RELEASE
QMAKE_CXXFLAGS_SHLIB	+= \$\$QMAKE_CFLAGS_SHLIB
QMAKE_CXXFLAGS_STATIC_LIB += \$\$QMAKE_CFLAGS_STATIC_LIB
QMAKE_CXXFLAGS_THREAD	+= \$\$QMAKE_CFLAGS_THREAD
QMAKE_CXXFLAGS_USE_PRECOMPILE = \$\$QMAKE_CFLAGS_USE_PRECOMPILE
QMAKE_CXXFLAGS_WARN_OFF	+= \$\$QMAKE_CFLAGS_WARN_OFF
QMAKE_CXXFLAGS_WARN_ON	+= \$\$QMAKE_CFLAGS_WARN_ON
QMAKE_CXXFLAGS_YACC	+= \$\$QMAKE_CFLAGS_YACC
QMAKE_CXX		= $PNDSDK/bin/arm-none-linux-gnueabi-g++
QMAKE_DEL_DIR         = rmdir
QMAKE_DEL_FILE        = rm -f
QMAKE_GZIP            = gzip -9f
QMAKE_INCDIR          = $PNDSDK/usr/include
QMAKE_INCDIR_OPENGL   = $PNDSDK/usr/include
QMAKE_INCDIR_OPENGL_ES1CL = \$\$QMAKE_INCDIR_OPENGL
QMAKE_INCDIR_OPENGL_ES1 = \$\$QMAKE_INCDIR_OPENGL
QMAKE_INCDIR_OPENGL_ES2 = \$\$QMAKE_INCDIR_OPENGL
QMAKE_INCDIR_EGL        = $PNDSDK/include
QMAKE_INCDIR_OPENVG   = $PNDSDK/usr/include/VG
QMAKE_INCDIR_QT       = $PNDSDK/usr/include/qt4
QMAKE_INCDIR_X11      = $PNDSDK/usr/include/X11
QMAKE_INCREMENTAL_STYLE = sublib
QMAKE_INSTALL_FILE    = install -m 644 -p
QMAKE_INSTALL_PROGRAM = install -m 755 -p
QMAKE_LEXFLAGS		+=
QMAKE_LEX		= flex
QMAKE_LFLAGS    += -L$PNDSDK/usr/lib -Wl,-rpath-link=$PNDSDK/usr/lib
QMAKE_LFLAGS    += -L$PNDSDK/lib -Wl,-rpath-link=$PNDSDK/lib
QMAKE_LFLAGS_APP	+=
QMAKE_LFLAGS_BSYMBOLIC_FUNC = -Wl,-Bsymbolic-functions
QMAKE_LFLAGS_DEBUG	+=
QMAKE_LFLAGS_DYNAMIC_LIST = -Wl,--dynamic-list,
QMAKE_LFLAGS_NOUNDEF	+= -Wl,--no-undefined
QMAKE_LFLAGS_RELEASE	+= -Wl,-O2
QMAKE_LFLAGS_SHLIB	+= -shared
QMAKE_LFLAGS_PLUGIN	+= \$\$QMAKE_LFLAGS_SHLIB
QMAKE_LFLAGS_SONAME	+= -Wl,-soname,
QMAKE_LFLAGS_THREAD	+=
QMAKE_LIBDIR          =
QMAKE_LIBDIR_OPENGL   = $PNDSDK/usr/lib
QMAKE_LIBDIR_OPENGL_ES1CL = \$\$QMAKE_LIBDIR_OPENGL
QMAKE_LIBDIR_OPENGL_ES1 = \$\$QMAKE_LIBDIR_OPENGL
QMAKE_LIBDIR_OPENGL_ES2 = \$\$QMAKE_LIBDIR_OPENGL
QMAKE_LIBDIR_EGL        = $PNDSDK/usr/lib
QMAKE_LIBDIR_OPENVG   =
QMAKE_LIBDIR_QT       = $PNDSDK/usr/lib
QMAKE_LIBDIR_X11      = $PNDSDK/usr/lib
QMAKE_LIBS            =
QMAKE_LIBS_DYNLOAD    = -ldl
QMAKE_LIBS_EGL        = -lEGL
QMAKE_LIBS_NIS        = -lnsl
QMAKE_LIBS_OPENGL_ES1CL = -lGLES_CL
QMAKE_LIBS_OPENGL_ES1 = -lGLES_CM
QMAKE_LIBS_OPENGL_ES2 = -lGLESv2
QMAKE_LIBS_OPENGL     = -lGLU -lGL
QMAKE_LIBS_OPENGL_QT  = -lGL
QMAKE_LIBS_OPENVG     = -lOpenVG
QMAKE_LIBS_THREAD     = -lpthread
QMAKE_LIBS_X11        = -lXext -lX11 -lm
QMAKE_LIBS_X11SM      = -lSM -lICE
QMAKE_LINK_C_SHLIB	= $PNDSDK/bin/arm-none-linux-gnueabi-gcc
QMAKE_LINK_C		= $PNDSDK/bin/arm-none-linux-gnueabi-gcc
QMAKE_LINK_SHLIB	= $PNDSDK/bin/arm-none-linux-gnueabi-g++
QMAKE_LINK		= $PNDSDK/bin/arm-none-linux-gnueabi-g++
QMAKE_MKDIR           = mkdir -p
QMAKE_MOC             = \$\$[QT_INSTALL_BINS]/$MOC_NAME
QMAKE_MOVE            = mv -f
QMAKE_OBJCOPY         = objcopy
QMAKE_PCH_OUTPUT_EXT    = .gch
QMAKE_CFLAGS_PRECOMPILE += -x c-header -c \${QMAKE_PCH_INPUT} -o \${QMAKE_PCH_OUTPUT}
QMAKE_CFLAGS_USE_PRECOMPILE += -include \${QMAKE_PCH_OUTPUT_BASE}
QMAKE_CXXFLAGS_PRECOMPILE += -x c++-header -c \${QMAKE_PCH_INPUT} -o \${QMAKE_PCH_OUTPUT}
QMAKE_RANLIB          =
QMAKE_LFLAGS_RPATH	= -Wl,-rpath,
QMAKE_STRIPFLAGS_LIB += --strip-unneeded
QMAKE_STRIP           = strip
QMAKE_TAR             = tar -cf
QMAKE_UIC             = \$\$[QT_INSTALL_BINS]/$UIC_NAME
QMAKE_YACCFLAGS		+= -d
QMAKE_YACCFLAGS_MANGLE  += -p \$base -b \$base
QMAKE_YACC_HEADER       = \$base.tab.h
QMAKE_YACC_SOURCE       = \$base.tab.c
QMAKE_YACC		= yacc
QT                     += core gui
QT_ARCH			= armv6
TEMPLATE                = app
QMAKE_PREFIX_STATICLIB  = lib
QMAKE_EXTENSION_STATICLIB = a

load(qt_config)

# example for using qmake with smplayer:
#make QMAKE="qmake -spec \$PNDSDK/../sdk_utils/qmake_linux-pandora-g++"
_END_

	cecho "Creating $USEFUL_TOOLS_DIR/qmake_linux-pandora-g++/qplatformdefs.h" $green
	# Create include required for qmake stuff
	cat > $USEFUL_TOOLS_DIR/qmake_linux-pandora-g++/qplatformdefs.h <<_END_
/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the qmake spec of the Qt Toolkit.
**
** \$QT_BEGIN_LICENSE:LGPL\$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** \$QT_END_LICENSE\$
**
****************************************************************************/

#include "/usr/share/qt4/mkspecs/linux-g++/qplatformdefs.h"

#define QT_GUI_DOUBLE_CLICK_RADIUS 20
_END_


	cecho "Created all additional files" $green
}


#
# Start of the "active" part of the script, everything above are just functions that will be called later on...
#


echo -e
cecho "Checking if all required programs are available in PATH" $green
for i in $REQUIRED_PROGS; do which $i &> /dev/null || report_absent_tool $i; done
echo -e
cecho "Found the following progs in PATH:" $green
cecho "$REQUIRED_PROGS" $green

# Creating temporary directory
echo -e
cecho "Creating temporary directory: $TMPDIR" $green
if [ ! -d "$TMPDIR" ]
then 
	mkdir -p $TMPDIR
fi

# basic codesourcery toolchain stuff
download_codesourcery_toolchain
install_codesourcery_toolchain

# get opkg up and running
download_opkg
install_opkg

# create the list of packages that should be installed next
gather_package_list

# install all the stuff that makes the SDK...
install_libs_via_opkg
# Crude hack around some packages (like eg libqtcore4) not being
# known/installed: just rerun the complete opkg part...
install_libs_via_opkg

# get pkg-config up and running
download_pkgconfig
install_pkgconfig

# fix some paths and such inside libraries and pkgconfig files
fix_libs

# provide some additional files
create_extra_files

# make sure that we are back where we started...
cd $CWD

# Add some things to $HOME/.bashrc if it is not available so far
grep -q "export PNDSDK=\"$PNDSDK\"" ~/.bashrc
if [ "$?" -ne "0" ];
then
	echo -e
	cecho "The following lines have been added to your login script (~/.bashrc):" $green
	# Add a default variable that allows to access the crosscompiler more
	# easily. Just use $PNDSDK/bin/pandora-gcc for calling the gcc!
	echo "export PNDSDK=\"$PNDSDK\""
	echo "export PNDSDK=\"$PNDSDK\"" >> ~/.bashrc
	# Append the binary paths from the sdk to the $PATH.
	#echo "export PATH=\"\$PATH:\$PNDSDK/bin:\$PNDSDK/usr/bin\""
	#echo "export PATH=\"\$PATH:\$PNDSDK/bin:\$PNDSDK/usr/bin\"" >> ~/.bashrc
	source ~/.bashrc
fi

# All done
echo -e
cecho "All done, your Pandora SDK is located here : $PNDSDK" $green
cecho "If you want to you can now remove the temporary directory ($TMPDIR). The files are only required if you plan to rerun this script to eg install a new version of the toolchain." $green


# this line is what is created as "opkg-target" by the openembedded toolchain(s)
# by now it is irrelevant, just access opkg using "$PNDSDK/../sdk_utils/opkg-pandora.sh" and be done!
# alias opkg-target='LD_LIBRARY_PATH=/usr/local/angstrom/arm/lib /usr/local/angstrom/arm/bin/opkg-cl -f /usr/local/angstrom/arm/arm-angstrom-linux-gnueabi/etc/opkg.conf -o /usr/local/angstrom/arm/arm-angstrom-linux-gnueabi'
