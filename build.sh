#!/bin/bash

echo "Building Sample"
cmake -S . -B build
cmake --build build --config release --target BasicCppSample