# Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
# SPDX-FileCopyrightText: 2023 
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.10)

# set the project name
project(example VERSION 0.0.1)

# add the executable
add_executable(example main.c)

if ("$ENV{OECORE_TARGET_ARCH}" STREQUAL "aarch64")
    target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib64/libpthread.so)
    target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib64/libdl.a)
elseif ("$ENV{OECORE_TARGET_ARCH}" STREQUAL "arm")
    target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libpthread.so)
    target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libdl.a)
elseif ("$ENV{OECORE_TARGET_ARCH}" STREQUAL "x86_64")
    target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib64/libpthread.so)
    target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib64/libdl.a)
else()
    message(FATAL_ERROR "missing architecture env variable or unknown value: OECORE_TARGET_ARCH=$ENV{OECORE_TARGET_ARCH}")
endif()
target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libmosquitto_static.a)

if ("$ENV{OECORE_TARGET_ARCH}" STREQUAL "x86_64")
    target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib64/libssl.a)
    target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib64/libcrypto.a)
else()
    target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libssl.a)
    target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libcrypto.a)
endif()

#target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libmosquitto.so)
#target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libpthread.so)
#target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libssl.a)
#target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libcrypto.a)
#target_link_libraries(example ${CMAKE_INSTALL_PREFIX}/lib/libdl.a)
