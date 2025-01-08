cmake_minimum_required(VERSION 3.19)
project(BasicCppLibraryProj)

set(LIBRARY_NAME BasicCppLibrary)

# Only builds the library target
set(BSC_LIB_ONLY false)

# GoogleTest requires at least C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
# Avoid warnings by using new policy
cmake_policy(SET CMP0135 NEW)


# ------ BasicCppLibrary ----- #

file(GLOB LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/BasicCppSample.cpp")

add_library(${LIBRARY_NAME} STATIC ${LIB_SOURCES})
set_target_properties(${LIBRARY_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${LIBRARY_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# Set the library version (optional)
set_target_properties(${LIBRARY_NAME} PROPERTIES VERSION 1.0 SOVERSION 1)

install(TARGETS ${LIBRARY_NAME} DESTINATION lib)


# ------ Google Test ----- #

if (NOT BSC_LIB_ONLY)

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
  )
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()

  file (GLOB TESTS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")
  add_executable(BasicCppTest ${TESTS})

  target_link_libraries(
    BasicCppTest
    GTest::gtest_main
    ${LIBRARY_NAME}
  )

  target_include_directories(BasicCppTest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/")

  include(GoogleTest)
  gtest_discover_tests(BasicCppTest)


  # ------ BasicCppSample ----- #

  project(BasicCppSample)
  file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/BasicCppSample.cpp" ${LIB_SOURCES})
  list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/BasicCppLibrary.cpp")

  add_executable(${PROJECT_NAME} ${SOURCES})

  target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
  target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARY_NAME})

endif()