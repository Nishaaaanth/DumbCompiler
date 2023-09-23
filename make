#! /bin/bash

if [ ! -d ./build ]; then
	mkdir build
fi

cmake -S . -B build
cmake --build build
build/dumb test.um
