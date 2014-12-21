MESSAGE(STATUS "Checking ${CMAKE_CURRENT_SOURCE_DIR} ")

# name of binary (without lib- prefix)
SET(LIBRARY_NAME "transmog")
# Interface/binary version
SET(LIBRARY_VERSION "0.2")

set(SRCS
   ${CMAKE_CURRENT_SOURCE_DIR}/libtransmog.c
)

# Create Package config
configure_file(
    lib${LIBRARY_NAME}.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/lib${LIBRARY_NAME}.pc
)

#-----
# Build binary
#-----
add_library(${LIBRARY_NAME} SHARED ${SRCS})


SET_TARGET_PROPERTIES(
  ${LIBRARY_NAME} PROPERTIES
  COMPILE_DEFINITIONS "TRANSMOG_MAGIC=${MAGIC_MOJO}"
  )

#-----
# Install
#-----
INSTALL(TARGETS ${LIBRARY_NAME} LIBRARY DESTINATION ${TARGET_LIB_INSTALL_DIR} )

INSTALL(DIRECTORY
   "${PROJECT_INCLUDE_DIR}/${LIBRARY_NAME}"
   DESTINATION ${TARGET_INC_INSTALL_DIR} FILES_MATCHING PATTERN "*.h")

# Install Pkg-config file
# pkg-config looks in /usr/lib/pkgconfig
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${LIBRARY_NAME}.pc
    DESTINATION ${TARGET_LIB_INSTALL_DIR}/pkgconfig/)
