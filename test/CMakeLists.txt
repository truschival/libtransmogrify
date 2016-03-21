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

if(NOT BUILD_GTEST_FROM_SRC)
  # Use prebuilt google_test
  find_package(GTest REQUIRED)

else(NOT BUILD_GTEST_FROM_SRC)
  # If Google Test is compiled from source
  include(ExternalProject)

  #--------------------------------------------------------------------------
  # Google Test as external project
  # Why - see http://code.google.com/p/googletest/wiki/FAQ#
  # How - borrowed form stackexchange
  #		(http://stackoverflow.com/questions/9689183/cmake-googletest)
  # http://stackoverflow.com/questions/9689183/cmake-googletest
  # http://www.kitware.com/media/html/BuildingExternalProjectsWithCMake2.8.html
  #--------------------------------------------------------------------------
  set(GTEST_PREFIX "${CMAKE_CURRENT_BINARY_DIR}")
  ExternalProject_Add(
	GTestExternal
	#URL https://github.com/google/googletest/archive/release-1.7.0.zip
	GIT_REPOSITORY https://github.com/google/googletest.git
	# URL_HASH
	# SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk/
	# SVN_REVISION -r 700
	TIMEOUT 10
	# PATCH_COMMMAND "" # Thomas: empty patch we dont patch google test
	# PATCH_COMMAND svn patch ${CMAKE_SOURCE_DIR}/gtest.patch
	#			${CMAKE_BINARY_DIR}/ThirdParty/src/googletest
	# Force separate output paths for debug and release builds to allow easy
	# identification of correct lib in subsequent TARGET_LINK_LIBRARIES commands
	CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
	-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
	-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
	-DBUILD_SHARED_LIBS=Off
	-Dgtest_disable_pthreads=On
	# -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
	# -Dgtest_force_shared_crt=ON
	PREFIX "${GTEST_PREFIX}"
	# Disable install step
	INSTALL_COMMAND ""
	# Empty update command to speed up make - we don't need to constantly check
 	UPDATE_COMMAND ""
 	)
	ExternalProject_Get_Property(GTestExternal source_dir)

	set(LIBPREFIX "${CMAKE_STATIC_LIBRARY_PREFIX}")
	set(LIBSUFFIX "${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set(GTEST_LOCATION "${GTEST_PREFIX}/src/GTestExternal-build")
	
	set(GTEST_INCLUDES "${source_dir}/googletest/include")
	set(GMOCK_INCLUDES "${source_dir}/googlemock/include")
	
	set(GTEST_LIBRARY  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${LIBPREFIX}gtest${LIBSUFFIX}")
	set(GTEST_MAINLIB  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${LIBPREFIX}gtest_main${LIBSUFFIX}")

	set(GMOCK_LIBRARY  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${LIBPREFIX}gmock${LIBSUFFIX}")
	set(GMOCK_MAINLIB  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${LIBPREFIX}gmock_main${LIBSUFFIX}")
	
	add_library(GTest IMPORTED STATIC GLOBAL)
	set_target_properties(GTest PROPERTIES
		IMPORTED_LOCATION       "${GTEST_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES     "${GTEST_INCLUDES};${GMOCK_INCLUDES}"
		IMPORTED_LINK_INTERFACE_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}")

	add_library(GTestMain IMPORTED STATIC GLOBAL)
	set_target_properties(GTestMain PROPERTIES
		IMPORTED_LOCATION 		"${GTEST_MAINLIB}"
		#INTERFACE_INCLUDE_DIRECTORIES      "${GTEST_INCLUDES}"
		IMPORTED_LINK_INTERFACE_LIBRARIES  GTest)

		
	add_library(GMock IMPORTED STATIC GLOBAL)
	set_target_properties(GMock PROPERTIES
		IMPORTED_LOCATION       "${GMOCK_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES     "${GTEST_INCLUDES};${GMOCK_INCLUDES}"
		IMPORTED_LINK_INTERFACE_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}")

	add_library(GMockMain IMPORTED STATIC GLOBAL)
	set_target_properties(GMockMain PROPERTIES
		IMPORTED_LOCATION 		"${GMOCK_MAINLIB}"
		#INTERFACE_INCLUDE_DIRECTORIES      "${GTEST_INCLUDES};${GMOCK_INCLUDES}"
		IMPORTED_LINK_INTERFACE_LIBRARIES  GMock )
			
	add_dependencies(GTest GTestExternal)
	add_dependencies(GMock GTestExternal)
	
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
  GMock
  GMockMain # pulls in gtest as linklibrary
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
