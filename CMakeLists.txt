cmake_minimum_required(VERSION 3.19)
project(BasicCppLibraryProj)

set(LIBRARY_NAME BasicCppLibrary)

# Check if this is included via FetchContent
if (NOT "${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    message(STATUS "${PROJECT_NAME} is included via FetchContent")
    set(BUILD_ALL_TARGETS FALSE)
else ()
    message(STATUS "${PROJECT_NAME} is the main project")
    set(BUILD_ALL_TARGETS TRUE)
endif ()

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
# Avoid warnings by using new policy
cmake_policy(SET CMP0135 NEW)


# ------ Google Test ----- #

if (BUILD_ALL_TARGETS)

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

endif()

# ------ BasicCppLibrary ----- #

file(GLOB LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/BasicCppSample.cpp")

add_library(${LIBRARY_NAME} STATIC ${LIB_SOURCES})
set_target_properties(${LIBRARY_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${LIBRARY_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# Set the library version (optional)
set_target_properties(${LIBRARY_NAME} PROPERTIES VERSION 1.0 SOVERSION 1)

install(TARGETS ${LIBRARY_NAME} DESTINATION lib)


# ------ BasicCppSample ----- #

if (BUILD_ALL_TARGETS)

  project(BasicCppSample)
  file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/BasicCppSample.cpp")

  add_executable(${PROJECT_NAME} ${LIB_SOURCES} ${SOURCES})

  target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
  target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARY_NAME})

endif()