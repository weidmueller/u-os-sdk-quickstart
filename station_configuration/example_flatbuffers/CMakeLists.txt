# Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.12)

project(example_fb
    VERSION 0.1.0
    LANGUAGES CXX
)

# Properties
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# includes
find_package(Flatbuffers REQUIRED)

# uc-slio-service api
add_subdirectory(api)

# executable
add_executable(example_fb
    main.cpp
    prepareRequestFromJSON.cpp
    transceive.cpp
    responseToJSON.cpp
    printBuffer.cpp
    printByteVect.cpp
    buildGetDeviceStateRequest.cpp
    buildDeployRequest.cpp
    responseToAccessors.cpp
)

# libraries
target_link_libraries(example_fb
  example_fb-flatbuffers
)

# include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
