list(APPEND CMAKE_MESSAGE_INDENT " ")
message(STATUS ">> ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_CURRENT_LIST_FILE}")

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

SET(DUT_LIBS "")
if(TARGET transmogrify)
  add_library(transmogrify::transmogrify ALIAS transmogrify)
  LIST(APPEND DUT_LIBS transmogrify::transmogrify)
endif()

SET(TEST_BINARY_NAME "${PROJECT_NAME}_gtest")

#--------------------------------------------------------------------------------
# UnitTests Executable (for target)
add_executable(${TEST_BINARY_NAME} ${TEST_HARNESS_SRCS} )

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
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


include(GoogleTest)
gtest_discover_tests(  ${TEST_BINARY_NAME}
  PROPERTIES
	XML_OUTPUT_DIR  "${CMAKE_BINARY_DIR}/testresults/"
  )

# -------------------------------------------------------------------
# COVERAGE LOGIC
# -------------------------------------------------------------------

# We check if coverage flags are present in the CXX_FLAGS 
# (which Conan injected via the profile)
string(FIND "${CMAKE_CXX_FLAGS}" "--coverage" HAS_GCOV)
string(FIND "${CMAKE_CXX_FLAGS}" "-fprofile-instr-generate" HAS_LLVM_COV)

if(HAS_GCOV GREATER -1 OR HAS_LLVM_COV GREATER -1)
    message(STATUS "Coverage instrumentation detected. Setting up report targets...")

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR HAS_GCOV GREATER -1)
        # --- GCC / LCOV / GCOVR Path ---
        find_program(GCOVR_PATH gcovr)
        find_program(LCOV_PATH lcov)

        if(GCOVR_PATH)
            add_custom_target(coverage_report
                COMMAND ${GCOVR_PATH} -r ${CMAKE_SOURCE_DIR} . 
                        --html-details -o coverage.html
                        --exclude-directories "${CMAKE_SOURCE_DIR}/test"
                        --exclude-directories "${CMAKE_SOURCE_DIR}/build"
                        --exclude ".*/test/.*"
                        --exclude "/usr/.*"
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating HTML report using gcovr"
            )
          message(STATUS "HTML report target created using gcovr")
        elseif(LCOV_PATH)
            add_custom_target(coverage_report
                # Capture coverage data
                COMMAND ${LCOV_PATH} --capture --directory . 
                        --ignore-errors inconsistent 
                        --output-file coverage.info
                # Remove unwanted files (STL, tests, dependencies)
                COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' '*/test/*' '*/_deps/*' '*/v1/*'
                        --ignore-errors unused 
                        --output-file coverage.info.filtered
                # Generate HTML report
                COMMAND genhtml coverage.info.filtered --output-directory coverage_report
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating HTML report using lcov"
            )
          message(STATUS "HTML report target created using lcov")
        endif()

    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # --- Native Clang / LLVM-COV Path ---
        find_program(LLVM_PROFDATA llvm-profdata)
        find_program(LLVM_COV llvm-cov)

        if(LLVM_PROFDATA AND LLVM_COV)
            # Note: default.profraw is generated when you RUN the binary
            add_custom_target(coverage_report
                # Merge the raw profile data
                COMMAND ${LLVM_PROFDATA} merge -sparse default.profraw -o coverage.profdata
                # Generate the HTML report
                COMMAND ${LLVM_COV} show $<TARGET_FILE:${TEST_BINARY_NAME}> 
                        -instr-profile=coverage.profdata 
                        -show-line-counts-or-regions
                        -ignore-filename-regex="/usr/include/.*|.*/test/.*"
                        -output-dir=coverage_report -format=html
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating HTML report using llvm-cov"
            )
          message(STATUS "HTML report target created using llvm-cov")
        endif()
    endif()
endif()

message(STATUS "<< ${CMAKE_CURRENT_LIST_FILE}")
list(POP_BACK CMAKE_MESSAGE_INDENT " ")
