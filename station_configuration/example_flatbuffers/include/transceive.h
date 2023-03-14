// Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
//
// SPDX-License-Identifier: Apache-2.0

#ifndef TRANSCEIVE_H_
#define TRANSCEIVE_H_

#include "iostream"
#include "flatbuffers/flatbuffers.h"

int transceive (uint8_t* raw_request_buffer, uint8_t* raw_response_buffer, uint resp_buf_size);

#endif