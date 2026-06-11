#!/bin/bash

echo "#include \"modules/modules.h\""

echo ""

echo "__attribute__((section(\".kernel_symbols\"))) struct symbol_table array[] = {"

tmp=$(cat | grep " [TDS] ")

echo "$tmp" | awk '{ printf "	{\"%s\", 0x%s},\n", $3, $1 }'

echo "};"

echo ""

tmp1=$(echo "$tmp" | wc -l)

echo "__attribute__((section(\".kernel_symbols\"))) uint32_t size = $tmp1;"

