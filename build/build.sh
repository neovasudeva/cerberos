#!/bin/bash

PREFIX="$HOME/opt/cross"
TARGET=x86_64-elf

GCC_VERSION=11.1.0
GCC_DIR=gcc-$GCC_VERSION

BINUTILS_VERSION=2.37
BINUTILS_DIR=binutils-$BINUTILS_VERSION
export PATH=$PATH:$PREFIX/bin

####################### pull gcc and binutils #######################
if [ $1 == "pull" ];
then
  wget https://ftp.gnu.org/gnu/binutils/binutils-2.37.tar.gz
  tar -xf binutils-2.37.tar.gz

  wget https://ftp.gnu.org/gnu/gcc/gcc-11.1.0/gcc-11.1.0.tar.gz
  tar -xf gcc-11.1.0.tar.gz
fi

####################### build binutils #######################

if [ $1 == "binutils" ];
then
  mkdir build-binutils
  cd build-binutils
  ../$BINUTILS_DIR/configure --target=$TARGET --prefix=$PREFIX --with-sysroot --disable-nls --disable-werror
  make
  make install
fi

######################## build gcc #######################

if [ $1 == "gcc" ];
then
  # The $PREFIX/bin dir _must_ be in the PATH. We did that above.
  which -- $TARGET-as || echo $TARGET-as is not in the PATH

  mkdir build-gcc
  cd build-gcc
  ../$GCC_DIR/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
  # remember that libgcc has 2 versions: one with red-zone and one with no-red-zone
  make all-gcc
  make all-target-libgcc
  make install-gcc
  make install-target-libgcc
fi

