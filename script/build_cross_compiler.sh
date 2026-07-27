#! /bin/bash

set -e
unset CC

export PREFIX="$HOME/sgoinfre/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

mkdir -p $HOME/sgoinfre
cd $HOME/sgoinfre

if [[ $1 == genext2fs ]]
then
	# install genext2fs
	git clone https://github.com/bestouff/genext2fs.git
	cd genext2fs
	./autogen.sh
	./configure --target=$TARGET --prefix="$PREFIX"
	make
	make install
	cd ../
	rm -rf genext2fs
else
	# install binutils
	wget https://ftp.gnu.org/gnu/binutils/binutils-2.46.0.tar.xz
	tar -xf binutils-2.46.0.tar.xz
	rm binutils-2.46.0.tar.xz

	mkdir -p build-binutils
	cd build-binutils
	../binutils-2.46.0/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
	make -j 6
	make install -j 6

	cd ../
	rm -rf build-binutils binutils-2.46.0

	# install gcc
	wget https://ftp.gnu.org/gnu/gcc/gcc-15.2.0/gcc-15.2.0.tar.xz
	tar -xf gcc-15.2.0.tar.xz
	rm gcc-15.2.0.tar.xz
	cd gcc-15.2.0
	./contrib/download_prerequisites
	cd ../

	mkdir -p build-gcc
	cd build-gcc
	../gcc-15.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c --without-headers --disable-hosted-libstdcxx
	make all-gcc -j 6
	make all-target-libgcc -j 6
	make install-gcc -j 6
	make install-target-libgcc -j 6

	cd ../
	rm -rf build-gcc gcc-15.2.0

	# install musl
	wget https://musl.libc.org/releases/musl-1.2.6.tar.gz
	tar -xzf musl-1.2.6.tar.gz

	mkdir -p musl-build
	cd musl-build

	../musl-1.2.6/configure --target=$TARGET --prefix="$PREFIX" --disable-libgloss
	make -j 6
	make install -j 6

	cd ../
	rm -rf musl-build musl-1.2.6.tar.gz
fi
