set(GSOAP_DIR "" CACHE PATH "gSOAP base location")

if(WIN32)
    set(GSOAP_PATH_SUFFIX "win32")
elseif(UNIX)
    set(GSOAP_PATH_SUFFIX "linux386")
endif()

find_program(GSOAP_SOAPCPP2
    NAMES soapcpp2.exe soapcpp2
    HINTS "${GSOAP_DIR}/bin"
    PATH_SUFFIXES ${GSOAP_PATH_SUFFIX}
    DOC "soapcpp2 tool location"
    )

find_program(GSOAP_WSDL2H
    NAMES wsdl2h.exe wsdl2h
    HINTS "${GSOAP_DIR}/bin"
    PATH_SUFFIXES ${GSOAP_PATH_SUFFIX}
    DOC "wsdl2h tool location"
    )

find_path(GSOAP_IMPORT_PATH
    NAMES wsa5.h
    HINTS "${GSOAP_DIR}/import"
    DOC "Location of standard gSOAP import modules"
    )

unset(GSOAP_PATH_SUFFIX)
mark_as_advanced(GSOAP_SOAPCPP2 GSOAP_WSDL2H GSOAP_IMPORT_PATH)

include(CMakeParseArguments)

function(gsoap_generate)
    set(options USE_DOM)
    set(oneValueArgs TYPEMAP GENERATE_HEADER SOURCE_HEADER OPTNAME OPTDESC OUTPUT_DIR)
    set(multiValueArgs SOURCES WSDL2H_FLAGS SOAPCPP2_FLAGS)
    cmake_parse_arguments(WSGEN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT WSGEN_GENERATE_HEADER OR NOT WSGEN_SOURCES OR NOT WSGEN_OPTNAME)
        message(FATAL_ERROR "Missing required parameters")
    endif()

    option(${WSGEN_OPTNAME} "${WSGEN_OPTDESC}" OFF)

    # Check if we're not asked to generate sources. Simply skip the rest of function.
    if(NOT ${${WSGEN_OPTNAME}})
        return()
    endif()

    # Sanity checks.
    if(NOT GSOAP_SOAPCPP2)
        message(FATAL_ERROR "soapcpp2 was not found, please check GSOAP_DIR variable")
    endif()
    if(NOT GSOAP_IMPORT_PATH)
        message(FATAL_ERROR "gSOAP import path was not found, please check GSOAP_DIR variable")
    endif()

    if(NOT GSOAP_WSDL2H)
        message(FATAL_ERROR "wsdl2h was not found, please check GSOAP_DIR variable")
    endif()

    message(STATUS "${WSGEN_OPTNAME}: Generating header file ${WSGEN_GENERATE_HEADER} from WSDLs")

    if(WSGEN_OUTPUT_DIR)
        set(WSGEN_GENERATE_HEADER "${WSGEN_OUTPUT_DIR}/${WSGEN_GENERATE_HEADER}")
    endif()

    set(WSDL2H_ARGS -g -c++11 -o "${WSGEN_GENERATE_HEADER}")

    if(WSGEN_TYPEMAP)
        list(APPEND WSDL2H_ARGS -t "${WSGEN_TYPEMAP}")
    endif()

    if(WSGEN_USE_DOM)
        list(APPEND WSDL2H_ARGS -d)
    endif()

    message(STATUS "Running ${GSOAP_WSDL2H} ${WSDL2H_ARGS};${WSGEN_WSDL2H_FLAGS};${WSGEN_SOURCES}")

    execute_process(
        COMMAND ${GSOAP_WSDL2H} ${WSDL2H_ARGS} ${WSGEN_WSDL2H_FLAGS} ${WSGEN_SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE WSDL2H_RESULT
        OUTPUT_VARIABLE WSDL2H_OUTPUT
        ERROR_VARIABLE WSDL2H_OUTPUT
    )

    message(STATUS "${WSDL2H_OUTPUT}")

    if(WSDL2H_RESULT GREATER 0)
        message(FATAL_ERROR "wsdl2h returned error")
    endif()

    if(NOT WSGEN_SOURCE_HEADER)
        set(WSGEN_SOURCE_HEADER "${WSGEN_GENERATE_HEADER}")
    endif()

    message(STATUS "${WSGEN_OPTNAME}: Generating source files from ${WSGEN_SOURCE_HEADER}")

    set(SOAPCPP2_ARGS -2 -S -c++11 -w -x -n -L -f200 "-I${GSOAP_IMPORT_PATH}")

    if(WSGEN_OUTPUT_DIR)
        list(APPEND SOAPCPP2_ARGS "-d${WSGEN_OUTPUT_DIR}" "-I${WSGEN_OUTPUT_DIR}")
    endif()

    message(STATUS "Running ${GSOAP_SOAPCPP2} ${SOAPCPP2_ARGS};${WSGEN_SOAPCPP2_FLAGS};${WSGEN_SOURCE_HEADER}")
    execute_process(
        COMMAND ${GSOAP_SOAPCPP2} ${SOAPCPP2_ARGS} ${WSGEN_SOAPCPP2_FLAGS} ${WSGEN_SOURCE_HEADER}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE SOAPCPP2_RESULT
        OUTPUT_VARIABLE SOAPCPP2_OUTPUT
        ERROR_VARIABLE SOAPCPP2_OUTPUT
    )

    message(STATUS "${SOAPCPP2_OUTPUT}")

    if(SOAPCPP2_RESULT GREATER 0)
        message(FATAL_ERROR "soapcpp2 returned error")
    endif()

    # Reset option in cache to prevent cycling generation.
    set(${WSGEN_OPTNAME} OFF CACHE BOOL "${WSGEN_OPTDESC}" FORCE)
endfunction()
