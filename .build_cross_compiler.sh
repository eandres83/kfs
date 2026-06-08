#! /bin/bash

export PREFIX="$HOME/sgoinfre/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

cd $HOME/sgoinfre

# install binutils
wget https://ftp.gnu.org/gnu/binutils/binutils-2.46.0.tar.xz
tar -xf binutils-2.46.0.tar.xz
rm binutils-2.46.0.tar.xz

mkdir build-binutils
cd build-binutils
../binutils-2.46.0/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make -j$(nproc)
make install -j$(nproc)

cd ../
rm -rf build-binutils binutils-2.46.0

# install gcc
wget https://ftp.gnu.org/gnu/gcc/gcc-15.2.0/gcc-15.2.0.tar.xz
tar --exclude='*testsuite*' --exclude='*/gcc/ada' --exclude='*/gcc/go' --exclude='*/gcc/d' --exclude='*/libgo' --exclude='*/libada' -xf gcc-15.2.0.tar.xz
rm gcc-15.2.0.tar.xz
cd gcc-15.2.0
./contrib/download_prerequisites
cd ../

mkdir build-gcc
cd build-gcc
../gcc-15.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c --without-headers --disable-hosted-libstdcxx
make all-gcc -j$(nproc)
make install-gcc -j$(nproc)

cd ../
rm -rf build-gcc gcc-15.2.0
