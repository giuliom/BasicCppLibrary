cmake_minimum_required(VERSION 3.19)
project(BasicCppLibrary)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
# Avoid warnings by using new policy
cmake_policy(SET CMP0135 NEW)


# ------ Google Test ----- #

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file (GLOB TESTS "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")
add_executable(BasicCppTest ${TESTS})

target_link_libraries(
  BasicCppTest
  GTest::gtest_main
)

target_include_directories(BasicCppTest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/")

include(GoogleTest)
gtest_discover_tests(BasicCppTest)


# ------ BasicCppLibrary ----- #

file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

add_library(${PROJECT_NAME} STATIC ${HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# Set the library version (optional)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION 1.0 SOVERSION 1)

install(TARGETS BasicCppLibrary DESTINATION lib)


# ------ BasicCppSample ----- #

project(BasicCppSample)
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${PROJECT_NAME} PUBLIC BasicCppLibrary)