#!/bin/bash

# ./doit.sh debug

cc="g++"
options="-I. -m32 -fPIC -Wall"
# -g -ggdb -O0 // debug build without optimization
# -Wno-write-strings // not full warnings

if [ "$1" == "debug" ] || [ "$2" == "debug" ]; then
	debug="-g -ggdb -O0"
else
	debug=""
fi
set -- "cod1_1_1"
constants="-D COD_VERSION=COD1_1_1"

mkdir -p bin
mkdir -p objects_$1

echo "##### COMPILE $1 CRACKING.CPP #####"
$cc $debug $options $constants -c cracking.cpp -o objects_$1/cracking.opp

echo "##### COMPILE $1 GSC.CPP #####"
$cc $debug $options $constants -c gsc.cpp -o objects_$1/gsc.opp

if  grep -q "COMPILE_ENTITY 1" config.hpp; then
	echo "##### COMPILE $1 GSC_ENTITY.CPP #####"
	$cc $debug $options $constants -c gsc_entity.cpp -o objects_$1/gsc_entity.opp
fi

if grep -q "COMPILE_PLAYER 1" config.hpp; then
	echo "##### COMPILE $1 GSC_PLAYER.CPP #####"
	$cc $debug $options $constants -c gsc_player.cpp -o objects_$1/gsc_player.opp
fi

echo "##### COMPILE $1 LIBCOD.CPP #####"
$cc $debug $options $constants -c libcod.cpp -o objects_$1/libcod.opp

echo "##### COMPILE $1 QVSNPRINTF.C #####"
$cc $debug $options $constants -c lib/qvsnprintf.c -o objects_"$1"/qvsnprintf.opp

echo "##### COMPILE $1 STRCMP_CONSTANT_TIME.C #####"
$cc $debug $options $constants -c lib/strcmp_constant_time.c -o objects_"$1"/strcmp_constant_time.opp

echo "##### LINKING lib$1.so #####"
objects="$(ls objects_$1/*.opp)"
$cc -m32 -shared -L/lib32 -o bin/lib$1.so -ldl $objects
rm objects_$1 -r

# Read leftover
#rm 0
