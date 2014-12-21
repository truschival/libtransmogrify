##
# Top-level CMakeLists.txt for libtransmog
#
#
# Possible BUILD_TYPE(s):
#  Debug  (default)
#  Release
#  RelWithDebInfo
#
# Options & Defaults (compilation flags & targets):
# -DBUILD_TEST=OFF	(build google test)
# -DBUILD_GTEST_FROM_SRC=Off (recompile gtest as external project)
# -DMAGIC_MOJO=19	(some compile time int)
# -DPROFILE=Off		(profiling)
# -DTEST_COVERAGE=Off	(code coverage)
# -DINSTALL_UNIT_TEST_ON_TARGET=Off (install unittest on target rootfs)
#
##

cmake_minimum_required (VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE  OFF)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_WARN_DEPRECATED ON)
# Add local macros for KBuild etc
list(APPEND CMAKE_MODULE_PATH  ${CMAKE_SOURCE_DIR}/CMakeModules/)

#-------------------------------------------------------------------------------
# Project Name
project (libtransmog)
# Project Version
SET(PROJECT_VERSION "0.1")
message(STATUS "")
message(STATUS "Running ${CMAKE_COMMAND} ${CMAKE_VERSION} on ${CMAKE_PROJECT_NAME} v${PROJECT_VERSION}" )

# If Google Test is compiled from source
include(ExternalProject)

# Package-Config for components that CMake does not support directly
include(FindPkgConfig)

# enable Cmake's make test
enable_testing()

#-------------------------------------------------------------------------------
# user Configuration + Defaults
#-------------------------------------------------------------------------------
option(BUILD_EXE "Build libtransmog-Tool" Off)
option(BUILD_TEST "Build Tests" Off)
option(BUILD_KERNEL_MODS "Build kernel modules" Off)
option(PROFILE "Build with Profiling" Off)
option(TEST_COVERAGE "Test Coverage" Off)
option(INSTALL_UNIT_TEST_ON_TARGET "Install unittests on target rootfs" Off)
option(BUILD_GTEST_FROM_SRC "Rebuild google test as external project" Off)

message(STATUS "Trying to setup or guess build-variables:")


#-------------------------------------------------------------------------------
# Command line parameters  or default
#-------------------------------------------------------------------------------
IF(NOT CMAKE_BUILD_TYPE)
    #Default to Release-Build
    SET(CMAKE_BUILD_TYPE Release
	CACHE STRING "Choose the type of build : Debug Release RelWithDebInfo"
	FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# Special transmogrification factor
if(NOT MAGIC_MOJO)
  SET(MAGIC_MOJO 23 CACHE INT "The magic mojo" FORCE )
endif(NOT MAGIC_MOJO)


#-------------------------------------------------------------------------------
# Print Summary
#-------------------------------------------------------------------------------

message(STATUS "------------- Config --------- " )
message(STATUS "Build-type:\t\t ${CMAKE_BUILD_TYPE} " )
message(STATUS "Build UnitTests:\t ${BUILD_TEST} ")
message(STATUS "Magic Mojo:\t\t ${MAGIC_MOJO}   ")
message(STATUS "Install UnitTests:\t ${INSTALL_UNIT_TEST_ON_TARGET} ")
message(STATUS "------------------------------ " )

#-------------------------------------------------------------------------------
# Input configuration
#-------------------------------------------------------------------------------

# Where to find headers for this project
SET(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# all include files
include_directories (${PROJECT_INCLUDE_DIR})

#-------------------------------------------------------------------------------
# Output  configuration
#-------------------------------------------------------------------------------
# Directory for executables during build
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
# Folder for libraries (during build)
SET(LIB_DIR	"${CMAKE_BINARY_DIR}/lib/")
# Where to put archives & shared libraries
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR})
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})

# Install paths on target
SET(TARGET_INC_INSTALL_DIR "include")
SET(TARGET_EXE_INSTALL_DIR "bin")
SET(TARGET_LIB_INSTALL_DIR "lib")

# Default name for debug builds (libraries)
SET(CMAKE_DEBUG_POSTFIX "_dbg")

#-------------------------------------------------------------------------------
# Compiler & Linker Flags
#-------------------------------------------------------------------------------
SET(GCC_WARNING_FLAGS   " -Wall -Wextra -fmessage-length=0 -Wno-unused-parameter " )

SET(CMAKE_C_FLAGS       " ${CMAKE_C_FLAGS} ${GCC_WARNING_FLAGS} ")
SET(CMAKE_CXX_FLAGS     " --std=c++11 ${CMAKE_CXX_FLAGS} ${GCC_WARNING_FLAGS} ")

SET(CMAKE_CXX_FLAGS_RELEASE  " ${CMAKE_CXX_FLAGS_RELEASE} -ffunction-sections -fdata-sections  --std=c++11")
SET(CMAKE_C_FLAGS_RELEASE  " ${CMAKE_C_FLAGS_RELEASE} -ffunction-sections -fdata-sections ")

# Link final Executable with Map, clean up unused sections
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE
    " ${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,--gc-sections -Wl,-Map,Symbols.map ")

if(PROFILE) # manually add -g because it could be a profile-release build
    SET(CMAKE_C_FLAGS      " ${CMAKE_C_FLAGS} -g  -pg ")
    SET(CMAKE_CXX_FLAGS    " ${CMAKE_CXX_FLAGS} -g -pg ")
    SET(CMAKE_LINK_FLAGS   " ${CMAKE_LINK_FLAGS} -O2 -g -pg ")
endif()

if(TEST_COVERAGE)
    SET(CMAKE_C_FLAGS     " -g -pg -O0 --coverage ${CMAKE_C_FLAGS} ")
    SET(CMAKE_CXX_FLAGS   " -g -pg -O0 --coverage ${CMAKE_CXX_FLAGS} ")
endif()


#-------------------------------------------------------------------------------
# Sub-Projects to build each with its own CMakelists.txt
#-------------------------------------------------------------------------------

# Sub-Projects
add_subdirectory(${PROJECT_SOURCE_DIR}/src/)

# Tests
if(BUILD_TEST)
  add_subdirectory(${PROJECT_SOURCE_DIR}/test/)
endif(BUILD_TEST)
