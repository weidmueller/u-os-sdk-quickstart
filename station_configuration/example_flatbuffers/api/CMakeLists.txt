# Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
#
# SPDX-License-Identifier: Apache-2.0

set(PARENT_PROJECT_NAME ${PROJECT_NAME})

project(${PROJECT_NAME}-flatbuffers
        VERSION ${PROJECT_VERSION}
        LANGUAGES CXX
        )

function(flatbuffers_generate_cpp)
    if(NOT ARGN)
        message(SEND_ERROR "Error: FLATBUFFERS_GENERATE_CPP() called without any proto files")
        return()
    endif()

    foreach(FIL ${ARGN})
        get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
        get_filename_component(FIL_WE ${FIL} NAME_WE)
        file(RELATIVE_PATH REL_FIL ${CMAKE_CURRENT_SOURCE_DIR} ${ABS_FIL})
        get_filename_component(REL_DIR ${REL_FIL} DIRECTORY)
        set(RELFIL_WE "${REL_DIR}/${FIL_WE}")

        set(_outputs
                "${CMAKE_CURRENT_BINARY_DIR}/${RELFIL_WE}.h"
                )

        add_custom_command(
                OUTPUT
                ${_outputs}
                COMMAND flatc
                ARGS
                --cpp
                --cpp-std c++17
                --filename-suffix ""
                --gen-mutable
                --no-prefix
                -o ${CMAKE_CURRENT_BINARY_DIR}
                ${REL_FIL}
                DEPENDS ${ABS_FIL}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Running C++ flatbuffers compiler on ${FIL}"
                VERBATIM
        )

        set_source_files_properties(${_outputs} PROPERTIES GENERATED TRUE)
    endforeach()
endfunction()

flatbuffers_generate_cpp(
        interface.fbs
)

add_library(${PROJECT_NAME} STATIC
        ${CMAKE_CURRENT_BINARY_DIR}/interface.h
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        )

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        flatbuffers::flatbuffers
        )