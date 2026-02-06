function(get_version_from_git_describe SEMVER_VERSION_STRING_FROM_GIT GIT_DESCRIBE_STRING_VAR) 
    find_package(Git REQUIRED)
    execute_process(
        COMMAND git describe --tags --long --abbrev=8 --always --dirty
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_DESCRIBE_REV
        OUTPUT_STRIP_TRAILING_WHITESPACE 
        RESULT_VARIABLE GIT_DESCRIBE_ERROR_CODE
        )

    if(GIT_DESCRIBE_ERROR_CODE)
        message(FATAL_ERROR "Error running git : ${GIT_DESCRIBE_ERROR_CODE}")
    else()
        # Check if it is at least Major.Minor.Patch.tweak + dirty and create project version
        # Figure out the version, strip the leading characters 'v'
        #                     prefix (major mandatory) (.minor) (.patch) (.tweak) (-rest)
        string(REGEX MATCH "^([A-Za-z]+)?([0-9]+)(\\.[0-9]+)?(\\.[0-9]+)?(\\.[0-9]+)?(-.*)?" 
                            SEMVER_MATCHES "${GIT_DESCRIBE_REV}")
    endif()

    if(NOT SEMVER_MATCHES)
        message(FATAL_ERROR "Cannot parse `git describe` ${GIT_DESCRIBE_REV} output to semver version! ")
    else()
        message(STATUS "Git project version ${GIT_DESCRIBE_REV} (matches: ${SEMVER_MATCHES})")
        # Version at least major version (any number after the optional leading characters)
        set(tmp_version ${CMAKE_MATCH_2})
        # Iterate over matches
        foreach(i RANGE 3 ${CMAKE_MATCH_COUNT})
            # Access each match using CMAKE_MATCH_<n>
            string(CONCAT match_var "CMAKE_MATCH_" ${i})
            if( "${${match_var}}" STREQUAL "")
                break()
            else()
                SET(tmp_version "${tmp_version}${${match_var}}")
            endif()
        endforeach()    
        MESSAGE(STATUS "** Project versions obtained from git: ${tmp_version} **" )
    endif()


    SET(${GIT_DESCRIBE_STRING_VAR} ${GIT_DESCRIBE_REV} PARENT_SCOPE)
    SET(${SEMVER_VERSION_STRING_FROM_GIT} ${tmp_version} PARENT_SCOPE)
endfunction()