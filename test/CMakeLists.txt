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
SET(DUT_LIBS
  transmog
  )
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

if(NOT BUILD_GTEST_FROM_SRC)
  # Use prebuilt google_test
  find_package(GTest REQUIRED)

else(NOT BUILD_GTEST_FROM_SRC)
  #--------------------------------------------------------------------------
  # Google Test as external project
  # Why - see http://code.google.com/p/googletest/wiki/FAQ#
  # How - borrowed form stackexchange
  #		(http://stackoverflow.com/questions/9689183/cmake-googletest)
  # http://www.kitware.com/media/html/BuildingExternalProjectsWithCMake2.8.html
  #--------------------------------------------------------------------------
  ExternalProject_Add(
	googletest
	SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk/
	SVN_REVISION -r 700
	TIMEOUT 10
	# PATCH_COMMMAND "" # Thomas: empty patch we dont patch google test
	# PATCH_COMMAND svn patch ${CMAKE_SOURCE_DIR}/gtest.patch
	#			${CMAKE_BINARY_DIR}/ThirdParty/src/googletest
	# Force separate output paths for debug and release builds to allow easy
	# identification of correct lib in subsequent TARGET_LINK_LIBRARIES commands
	#CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
	# -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=DebugLibs
	# -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=ReleaseLibs
	# -Dgtest_force_shared_crt=ON
	# Disable install step
	INSTALL_COMMAND ""
	# Empty update command to speed up make - we don't need to constantly check
 	UPDATE_COMMAND ""
 	)
  ExternalProject_Get_Property(googletest source_dir)
  include_directories(${source_dir}/include)
  ExternalProject_Get_Property(googletest binary_dir)
  set(GTEST_LIBRARIES ${binary_dir}/libgtest.a)
  #Could also be your own main.cpp
  set(GTEST_MAIN_LIBRARIES ${binary_dir}/libgtest_main.a)
  # Executable depends in this case on building external project
  add_dependencies(${TEST_BINARY_NAME} googletest)
endif(NOT BUILD_GTEST_FROM_SRC)

# Linkage with gtest_main coverage etc.
TARGET_LINK_LIBRARIES(
  ${TEST_BINARY_NAME}
  ${DUT_LIBS}   # Units under test
  ${GTEST_LIBRARIES}
  ${GTEST_MAIN_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  )


SET_TARGET_PROPERTIES(
  ${TEST_BINARY_NAME} PROPERTIES
  COMPILE_DEFINITIONS "TRANSMOG_MAGIC=${MAGIC_MOJO}"
  )


if(TEST_COVERAGE)
  TARGET_LINK_LIBRARIES (${TEST_BINARY_NAME}  ${COVERAGE_LIB} )
endif(TEST_COVERAGE)

# Call the testBinary with junit-xml output
add_test(junitout "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_BINARY_NAME}"
  --gtest_output=xml:gtestresults.xml )

if(INSTALL_UNIT_TEST_ON_TARGET)
  INSTALL(TARGETS  ${TEST_BINARY_NAME} RUNTIME DESTINATION ${TARGET_EXE_INSTALL_DIR} )
endif(INSTALL_UNIT_TEST_ON_TARGET)

