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
# UnitTests Executable (for target)
add_executable(${TEST_BINARY_NAME} ${TEST_HARNESS_SRCS} )

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

SET_TARGET_PROPERTIES(
  ${TEST_BINARY_NAME}
  PROPERTIES
  COMPILE_DEFINITIONS "TRANSMOG_MAGIC=${MAGIC_MOJO}"
  )


# Linkage with gtest_main coverage etc.
TARGET_LINK_LIBRARIES(
  ${TEST_BINARY_NAME}
  ${DUT_LIBS}   # Units under test
  GTest::gtest_main
  )


if(TEST_COVERAGE)
  TARGET_LINK_LIBRARIES (${TEST_BINARY_NAME}  ${COVERAGE_LIB} )
endif(TEST_COVERAGE)

include(GoogleTest)
gtest_discover_tests(  ${TEST_BINARY_NAME}
  PROPERTIES
	XML_OUTPUT_DIR  "${CMAKE_BINARY_DIR}/testresults/"
  )

if(INSTALL_UNIT_TEST_ON_TARGET)
  INSTALL(TARGETS  ${TEST_BINARY_NAME} RUNTIME DESTINATION ${TARGET_EXE_INSTALL_DIR} )
endif(INSTALL_UNIT_TEST_ON_TARGET)
