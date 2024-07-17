cmake_minimum_required(VERSION 3.19)
project(BasicCppSample)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BSC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/BasicCppLibrary")

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

add_executable(${PROJECT_NAME} ${SOURCES})

add_subdirectory(${BSC_PATH})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${BSC_PATH}/include/")

target_link_libraries(${PROJECT_NAME} PUBLIC BasicCppLibrary)

add_dependencies(${PROJECT_NAME} BasicCppLibrary)