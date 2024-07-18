#!/bin/bash

echo "Building Sample"
cmake -S . -B build
cmake --build build --target BasicCppSample