MESSAGE(STATUS "Checking ${CMAKE_CURRENT_SOURCE_DIR} ")

# Threading library for gtest
# Use ${CMAKE_THREAD_LIBS_INIT} for the library
find_package(Threads REQUIRED)

#--------------------------------------------------------------------------------
# Unit test sources, TestDoubles, Mocks etc.
#--------------------------------------------------------------------------------
SET(TEST_HARNESS_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
  )

#--------------------------------------------------------------------------------
# Add libraries to test here
#--------------------------------------------------------------------------------
#------------------------------
# Import Arduino core
#------------------------------

SET(DUT_LIBS "")
if(TARGET transmog)
  add_library(LibTransmog::transmog ALIAS transmog)
else()
    find_package(LibTransmog COMPONENTS transmog CONFIG REQUIRED)
endif()
LIST(APPEND DUT_LIBS LibTransmog::transmog)

SET(TEST_BINARY_NAME "${PROJECT_NAME}_gtest")

#--------------------------------------------------------------------------------
# Google Test as external project
# Why - see http://code.google.com/p/googletest/wiki/FAQ#
# How - borrowed form stackexchange
#		(http://stackoverflow.com/questions/9689183/cmake-googletest)
# http://www.kitware.com/media/html/BuildingExternalProjectsWithCMake2.8.html
#--------------------------------------------------------------------------------
# UnitTests Executable (for target)
add_executable(${TEST_BINARY_NAME} ${TEST_HARNESS_SRCS} )

# --------------------------------------------------------------------------------
if(NOT BUILD_GTEST_FROM_SRC)
  #Use prebuilt google_test
  find_package(GTest REQUIRED)

else(NOT BUILD_GTEST_FROM_SRC)
  # Great idea how to download and configure gtest at configuration time
  # taken form http://crascit.com/2015/07/25/cmake-gtest/
#-- Snip
  # Download and unpack googletest at configure time
  configure_file(CMakeLists.txt.in
    ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt)

  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}"  --build .
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )

  execute_process(COMMAND ${CMAKE_COMMAND} --build .
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )

  # Prevent GoogleTest from overriding our compiler/linker options
  # when building with Visual Studio
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # Add googletest directly to our build. This adds
  # the following targets: gtest, gtest_main, gmock
  # and gmock_main
  add_subdirectory(
	  ${CMAKE_BINARY_DIR}/googletest-src
      ${CMAKE_BINARY_DIR}/googletest-build)
#-- snap

endif(NOT BUILD_GTEST_FROM_SRC)


SET_TARGET_PROPERTIES(
  ${TEST_BINARY_NAME}
  PROPERTIES
  COMPILE_DEFINITIONS "TRANSMOG_MAGIC=${MAGIC_MOJO}"
  )


# Linkage with gtest_main coverage etc.
TARGET_LINK_LIBRARIES(
  ${TEST_BINARY_NAME}
  ${DUT_LIBS}   # Units under test
  gtest
  gmock
  gmock_main
  )



##
# MSVC does not link with libmath
# Coverage support only for gnu gcov
##
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  target_link_libraries(  ${TEST_BINARY_NAME}
	m #link libmath
  )

set_target_properties( ${TEST_BINARY_NAME}
    PROPERTIES
	COMPILE_FLAGS " -pg --coverage "
	LINK_FLAGS " -pg -O0 --coverage  "
	)
endif()

# Call the testBinary with junit-xml output
add_test(junitout "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_BINARY_NAME}"
  --gtest_output=xml:gtestresults.xml )

if(INSTALL_UNIT_TEST_ON_TARGET)
  INSTALL(TARGETS  ${TEST_BINARY_NAME} RUNTIME DESTINATION ${TARGET_EXE_INSTALL_DIR} )
endif(INSTALL_UNIT_TEST_ON_TARGET)
