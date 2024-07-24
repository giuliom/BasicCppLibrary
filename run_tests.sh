#!/bin/bash

cmake -S . -B build
cmake --build build --config release
cd build
ctest -C release --output-on-failure