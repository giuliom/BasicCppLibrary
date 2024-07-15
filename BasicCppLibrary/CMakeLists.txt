cmake_minimum_required(VERSION 3.14)
project(BasicCppLibrary)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file (GLOB TESTS "test/*.h" "test/*.cpp")
add_executable(LibTest ${TESTS})

target_link_libraries(
  LibTest
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(LibTest)

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB SOURCES "source/*.cpp" "include/*.h")

add_executable(BasicCppDemo ${SOURCES})

# Add the library target
add_library(BasicCppLibrary STATIC source/BasicCppLibrary.cpp)

# Specify the include directory for the target
target_include_directories(BasicCppLibrary PUBLIC include)

# Platform-specific settings
if(WIN32)
    message(STATUS "Configuring for Windows")
elseif(UNIX)
    message(STATUS "Configuring for Linux")
endif()

# Install rules
install(TARGETS BasicCppLibrary DESTINATION lib)