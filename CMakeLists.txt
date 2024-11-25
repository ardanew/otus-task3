cmake_minimum_required(VERSION 3.10)

if(DEFINED ENV{GITHUB_RUN_NUMBER})
  set(PATCH_NUM $ENV{GITHUB_RUN_NUMBER})
else()
  set(PATCH_NUM 0)
endif()

project(allocator
  LANGUAGES CXX
  VERSION 0.0.${PATCH_NUM}
)

if(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()
message(STATUS "CMake build type = ${CMAKE_BUILD_TYPE} platform-dir=${PLATFORM_BUILD_DIR}")

configure_file(version.h.in version.h)

include_directories(${CMAKE_BINARY_DIR})
add_subdirectory(src)

option(BUILD_TESTS "Build unittests" OFF)
#if(BUILD_TESTS)
#    enable_testing()
#    add_subdirectory(tests)
#endif()

option(GENERATE_DEB OFF)
message(STATUS "Generate DEB file = ${GENERATE_DEB}")
if(GENERATE_DEB)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
  set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
  set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
  SET(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_SOURCE_DIR}/output/${PLATFORM_BUILD_DIR}/packages")
  set(CPACK_PACKAGE_CONTACT example@example.com)
  set(CPACK_DEB_COMPONENT_INSTALL ON)
  set(CPACK_COMPONENTS_ALL app) # to exclude tests
  set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
  include(CPack)
endif()

