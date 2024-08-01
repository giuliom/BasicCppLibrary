# Basic Cpp Library

![Build and Tests](https://github.com/giuliom/BasicCppLibrary/actions/workflows/CI.yml/badge.svg?branch=main)

## Description

The library is not production ready and its development is currently on hold.

A C++ collection of useful or custom methods and classes covering these topics:
- Algorithms (Sorting and Search)
- Chrono
- Concurrency
- Enum Utilities
- Math
- Memory and Smart Pointers
- Regex


## Getting Started

The library is cross-platofrm and fully supports Cmake.
If you want to generate a Visual Studio solution run shell script `generate_vs_sln.sh`

- Install Cmake
- Run shell scripts `build.sh` and `run_tests.sh` to build the project and run the tests
- There are 3 CMake targets:
  - The library `BasicCppLibrary`
  - The sample executable `BasicCppSample`
  - The Google Test executable `BasicCppTest`

It is recommended to consume the library using CMake's [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html)
