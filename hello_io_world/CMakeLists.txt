# Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
# SPDX-FileCopyrightText: 2023 
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.10)

SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a") 
SET(BUILD_SHARED_LIBS OFF) 
SET(CMAKE_EXE_LINKER_FLAGS "-static") 

# set the project name
project(example_io VERSION 0.0.1)

# add the executable
add_executable(example_io main_triple.c)

# uncomment this to link statically, e.g. for use in docker container
#SET_TARGET_PROPERTIES (example_io PROPERTIES LINK_FLAGS "-static")

# set the C/C++ standard
set_property(TARGET example_io PROPERTY C_STANDARD 90)

#define _GNU_SOURCE
target_compile_definitions(example_io PRIVATE _GNU_SOURCE)
