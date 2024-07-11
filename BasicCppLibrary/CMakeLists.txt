cmake_minimum_required(VERSION 3.14)
project(BasicCppLibrary)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB SOURCES "source/*.cpp" "include/*.h")

add_executable(BasicCppLibrary ${SOURCES})