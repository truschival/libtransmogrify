list(APPEND CMAKE_MESSAGE_INDENT " ")
message(STATUS ">> ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_CURRENT_LIST_FILE}")

# name of library (without lib- prefix)
set(LIBRARY_NAME "transmogrify")

#includes etc. in folder transmog
set(COMPONENT_PATH "transmogrify")

# Interface/binary version
set(LIBRARY_VERSION ${PROJECT_VERSION})
# add . to the includes during the build
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#------------------------------
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in" "${GENERATED_DIR}/config.h"
        ESCAPE_QUOTES @ONLY NEWLINE_STYLE UNIX)

#------------------------------
# normal sources
#------------------------------
set(SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/libtransmog.c
)

set(LIBRARY_COMPILE_DEFINITIONS "TRANSMOG_MAGIC=${MAGIC_MOJO}")

#------------------------------
# Output a library
#------------------------------
add_library(${LIBRARY_NAME} ${SRCS})

set_target_properties(
        ${LIBRARY_NAME} PROPERTIES
        VERSION ${LIBRARY_VERSION}
        SOVERSION ${LIBRARY_VERSION}
)

# Set compile definitions for both library and consumers
target_compile_definitions(${LIBRARY_NAME}
                           PUBLIC
                           "FOO_BAR=1"
                           PRIVATE
                           "TRANSMOG_MAGIC=${MAGIC_MOJO}"
)

target_include_directories(
        ${LIBRARY_NAME}
        PRIVATE
        $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>
)

if(CMAKE_COMPILER_IS_GNUXX)
    target_link_libraries(${LIBRARY_NAME}
                          m #link libmath
    )
endif(CMAKE_COMPILER_IS_GNUXX)

#------------------------------
# Install targets
#------------------------------
# Install the library with export information
install(TARGETS ${LIBRARY_NAME}
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${INSTALL_BIN_DIR}
)

# Install headers
install(DIRECTORY
        ${PROJECT_INCLUDE_DIR}/
        DESTINATION ${INSTALL_INCLUDE_DIR}/${COMPONENT_PATH}
)

message(STATUS "<< ${CMAKE_CURRENT_LIST_FILE}")
list(POP_BACK CMAKE_MESSAGE_INDENT " ")
