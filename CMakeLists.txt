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
# -DMAGIC_MOJO=19	(some compile time int)
# -DPROFILE=Off		(profiling)
# -DTEST_COVERAGE=Off	(code coverage)
# -DINSTALL_UNIT_TEST_ON_TARGET=Off (install unittest on target rootfs)
#
##

cmake_minimum_required(VERSION 3.15)

# When using 'conan create', the toolchain is automatically passed via -DCMAKE_TOOLCHAIN_FILE
# No manual include needed as it's handled by Conan's CMake integration
# The toolchain must be set before project() for proper compiler detection

set(CMAKE_CXX_STANDARD 17) # GTest needs at least C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE On)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_WARN_DEPRECATED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_policy(SET CMP0135 NEW) # DOWNLOAD_EXTRACT_TIMESTAMP is NEW

# Add local macros for KBuild etc
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules/)

# Check if version variables are provided from command line (e.g., by Conan)
if(DEFINED CONAN_PROJECT_VERSION AND DEFINED CONAN_GIT_DESCRIBE_REV)
    set(PROJECT_VERSION ${CONAN_PROJECT_VERSION})
    set(GIT_DESCRIBE_REV ${CONAN_GIT_DESCRIBE_REV})
    message(STATUS "Using version from command line: ${PROJECT_VERSION} (${GIT_DESCRIBE_REV})")
else()
    # Fallback: Semver version from git describe
    include(semVerVersionFromGit)
    get_version_from_git_describe(PROJECT_VERSION GIT_DESCRIBE_REV)
    message(STATUS "Using version from git: ${PROJECT_VERSION} (${GIT_DESCRIBE_REV})")
endif()

string(TIMESTAMP CMAKE_CONFIGURATION_TIME "%Y-%m-%d %H:%M:%S" UTC)
#-------------------------------------------------------------------------------
# Project Name
project(libtransmogrify VERSION ${PROJECT_VERSION} LANGUAGES CXX C)

# Some arbitrary project description - only used in doxygen and cpack
set(PROJECT_DESCRIPTION "Playing around with CMake and GoogleTest")
message(STATUS
        "Running ${CMAKE_COMMAND} ${CMAKE_VERSION} on ${CMAKE_PROJECT_NAME} v${PROJECT_VERSION}")

#-------------------------------------------------------------------------------
# user Configuration + Defaults
#-------------------------------------------------------------------------------
option(BUILD_TEST "Build Tests" Off)
option(PROFILE "Build with Profiling" Off)
option(TEST_COVERAGE "Test Coverage" Off)
option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." Off)

#-------------------------------------------------------------------------------
# Command line parameters  or default
#-------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    #Default to Release-Build
    set(CMAKE_BUILD_TYPE Release
        CACHE STRING "Choose the type of build : Debug Release RelWithDebInfo"
        FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# Special transmogrification factor
if(NOT MAGIC_MOJO)
    set(MAGIC_MOJO 23 CACHE INT "The magic mojo" FORCE)
endif(NOT MAGIC_MOJO)

# User did not provide install prefix via cmake commandline
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${STAGING_DIR})
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

#-------------------------------------------------------------------------------
# Print Summary
#-------------------------------------------------------------------------------

message(STATUS "------------- Config --------- ")
message(STATUS "Build-type:\t\t ${CMAKE_BUILD_TYPE} ")
message(STATUS "Build UnitTests:\t ${BUILD_TEST} ")
message(STATUS "Magic Mojo:\t\t ${MAGIC_MOJO}   ")
message(STATUS "------------------------------ ")

#-------------------------------------------------------------------------------
# Input configuration
#-------------------------------------------------------------------------------

# Where to find headers for this project
set(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Cmake generated configuration
# Output directories for generated files
set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Find includes in corresponding build directories
include_directories(${PROJECT_INCLUDE_DIR} ${GENERATED_DIR})

#-------------------------------------------------------------------------------
# Output configuration
#-------------------------------------------------------------------------------

# (CMAKE_BINARY_DIR = where cmake was invoked and found project())
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin/")
# Folder for libraries (during build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib/")
# Put Archives + Libraries together
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
set(INSTALL_DOC_DIR "doc/${PROJECT_NAME}" CACHE PATH "Installation directory for doxygen docs")


#-------------------------------------------------------------------------------
# Compiler & Linker Flags
#-------------------------------------------------------------------------------
# Most compiler flags are now handled by Conan toolchain and profiles
# Only keep project-specific flags here

# Debug postfixes for target libraries
set(CMAKE_DEBUG_POSTFIX "_dbg")
set(CMAKE_TESTCOVERAGE_POSTFIX "_cov")

# Profiling support
if(PROFILE) # manually add -g because it could be a profile-release build
    set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -g  -pg ")
    set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -g -pg ")
    set(CMAKE_LINK_FLAGS " ${CMAKE_LINK_FLAGS} -O2 -g -pg ")
endif()

# Test coverage support (if not handled by Conan)
if(TEST_COVERAGE AND NOT CONAN_CMAKE_CXX_FLAGS)
    include(CodeCoverage)
    append_coverage_compiler_flags()

    if(CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang"
            OR CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?[Cc]lang")
        set(GCOV_EXECUTABLE "llvm-cov")
        set(COVERAGE_LIB "llvm-cov")

    elseif(CMAKE_C_COMPILER_ID MATCHES "GNU"
            OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(GCOV_EXECUTABLE "gcov")
        set(COVERAGE_LIB "gcov")
        message(STATUS "Code coverage enabled, using coverage library: ${COVERAGE_LIB} ")

    endif()
endif(TEST_COVERAGE AND NOT CONAN_CMAKE_CXX_FLAGS)

#-------------------------------------------------------------------------------
# Generate documentation
#-------------------------------------------------------------------------------
find_package(Doxygen)
if(DOXYGEN_FOUND AND BUILD_DOCS)
    set(DOXYGEN_DOC_DIR_NAME "Doc")
    # Generate a doxyfile for complete documentation
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
                   ${GENERATED_DIR}/Doxyfile @ONLY)

    # detailed module doumentation is not always generated (no ALL after target)
    add_custom_target(DOC
                      ${DOXYGEN_EXECUTABLE} ${GENERATED_DIR}/Doxyfile
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Generate documentation" VERBATIM
    )

    # install all files flat into /doc/${PROJECT_NAME}/
    install(DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}/${DOXYGEN_DOC_DIR_NAME}/html/
            COMPONENT APIDOC
            DESTINATION ${INSTALL_DOC_DIR}
    )
endif(DOXYGEN_FOUND)

#-------------------------------------------------------------------------------
# Sub-Projects to build each with its own CMakelists.txt
#-------------------------------------------------------------------------------

# Sub-Projects
add_subdirectory(${PROJECT_SOURCE_DIR}/src/)

# Tests
if(BUILD_TEST)
    # enable Cmake's make test
    enable_testing()
    add_subdirectory(${PROJECT_SOURCE_DIR}/test/)
endif(BUILD_TEST)

#-------------------------------------------------------------------------------
message(STATUS "<< ${CMAKE_CURRENT_LIST_FILE}")
list(POP_BACK CMAKE_MESSAGE_INDENT " ")
