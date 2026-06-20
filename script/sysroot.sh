#! /bin/bash

mkdir -p "$1/home" "$1/home/kfs" "$1/home/eandres"
mkdir -p "$1/root"
mkdir -p "$1/bin"
mkdir -p "$1/dev"
mkdir -p "$1/etc"
mkdir -p "$1/proc"
mkdir -p "$1/sbin"
mkdir -p "$1/usr"
mkdir -p "$1/tmp"
mkdir -p "$1/sys"
mkdir -p "$1/var"
mkdir -p "$1/boot"

echo -n "root:x:0:0:root:/root:/bin/minishell\neandres:x:1000:1000:user:/home/eandres:/bin/minishell" > $1/etc/passwd

echo -n "root:1\neandres:1234" > $1/etc/shadow
#chmod 400 $1/etc/shadow

echo -n "Mierdon\n" > $1/dev/file.txt
echo -n "Hola desde el dico duro" > $1/home/kfs/file.txt
