#!/bin/bash

SCRIPT_DIR="$(cd -P -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"

cd SCRIPT_DIR

rm -rf build build-mips

mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug || exit 1
make || exit 1

cd ..

mkdir -p build-mips
cd build-mips
cmake .. --toolchain toolchain.cmake -DCMAKE_BUILD_TYPE=Release || exit 1
make || exit 1

