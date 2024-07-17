cmake_minimum_required(VERSION 3.19)
project(BasicCppLibrary)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Avoid warnings by using new policy
cmake_policy(SET CMP0135 NEW)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file (GLOB TESTS "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")
add_executable(LibTest ${TESTS})

target_link_libraries(
  LibTest
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(LibTest)

# Library setup
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

# Add the library target
add_library(${PROJECT_NAME} SHARED ${SOURCES}) # interface for header only libraries

# Specify the include directory for the target
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# Platform-specific settings
if(WIN32)
    message(STATUS "Configuring for Windows")
elseif(UNIX)
    message(STATUS "Configuring for Linux")
endif()

# Install rules
install(TARGETS BasicCppLibrary DESTINATION lib)