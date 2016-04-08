MESSAGE(STATUS "Checking ${CMAKE_CURRENT_SOURCE_DIR} ")

# name of binary (without lib- prefix)
SET(LIBRARY_NAME "transmog")
#Since the "standard" cmake template works with component name set it
set(COMPONENT_NAME ${PROJECT_NAME})

#includes etc. in folder transmog
SET(COMPONENT_PATH "transmog")

# Interface/binary version
SET(LIBRARY_VERSION ${PROJECT_VERSION})
# add . to the includes during the build
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#------------------------------
# normal sources
#------------------------------
set(SRCS
   ${CMAKE_CURRENT_SOURCE_DIR}/libtransmog.c
)

SET(LIBRARY_COMPILE_DEFINITIONS "TRANSMOG_MAGIC=${MAGIC_MOJO}")

#------------------------------
# Output a library
#------------------------------
ADD_LIBRARY(${LIBRARY_NAME} ${SRCS})

set_target_properties(
  ${LIBRARY_NAME} PROPERTIES
  VERSION ${LIBRARY_VERSION}
  SOVERSION ${LIBRARY_VERSION}
  )

target_compile_definitions(${LIBRARY_NAME}
  PRIVATE
  $<BUILD_INTERFACE:${LIBRARY_COMPILE_DEFINITIONS}>
  PUBLIC
  $<BUILD_INTERFACE:FOO_BAR=1>
  )

target_include_directories(
  ${LIBRARY_NAME}
  PRIVATE
  $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>
)


##
# MSVC does not link with libmath
# Coverage support only for gnu gcov
##
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")

  Find_library(COVERAGE_LIB gcov)

  target_link_libraries(${LIBRARY_NAME}
	m #link libmath
	)

  if(TEST_COVERAGE)
	set_target_properties(${LIBRARY_NAME}
      PROPERTIES
	  COMPILE_FLAGS " -pg --coverage "
	  LINK_FLAGS " -pg -O0 --coverage  "
	  )
  endif(TEST_COVERAGE)
endif()

#-----
# Install
#-----
MESSAGE(STATUS "** Generating Package Configurations **")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${version_config}
  VERSION ${LIBRARY_VERSION}
  COMPATIBILITY SameMajorVersion
)


# Create Package config (obsolete)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/lib${LIBRARY_NAME}.pc.in
    lib${LIBRARY_NAME}.pc
)

# Configure '<PROJECT-NAME>Config.cmake'
# Note: variable 'targets_export_name' used
configure_file(
  "${CMAKE_SOURCE_DIR}/cmake/Config.cmake.in"
  "${component_config}"
  @ONLY
  )

INSTALL(TARGETS ${LIBRARY_NAME}
  EXPORT ${targets_export_name}
  COMPONENT DEVELOP
  ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
  LIBRARY DESTINATION ${INSTALL_LIB_DIR}
  RUNTIME DESTINATION ${INSTALL_BIN_DIR}
  # this will add -I<prefix>include/transmog to client compile flags
  #INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}/${COMPONENT_PATH}
  INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}
  )


INSTALL(DIRECTORY
  ${PROJECT_INCLUDE_DIR}/
  COMPONENT DEVELOP
  DESTINATION ${INSTALL_INCLUDE_DIR}/${COMPONENT_PATH}
  )

INSTALL(
  EXPORT ${targets_export_name}
  COMPONENT DEVELOP
  NAMESPACE "${COMPONENT_NAME}::"
  DESTINATION "${INSTALL_CMAKE_DIR}/${COMPONENT_NAME}"
  )

INSTALL(
  FILES "${component_config}" "${version_config}"
  COMPONENT DEVELOP
  DESTINATION "${INSTALL_CMAKE_DIR}/${COMPONENT_NAME}"
  )

# Install Pkg-config file
# pkg-config looks in /usr/lib/pkgconfig
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${LIBRARY_NAME}.pc
    COMPONENT DEVELOP
    DESTINATION ${INSTALL_LIB_DIR}/pkgconfig/)
