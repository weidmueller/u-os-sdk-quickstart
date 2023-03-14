// Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
//
// SPDX-License-Identifier: Apache-2.0

#ifndef PRINTBYTEVECT_H_
#define PRINTBYTEVECT_H_

#include "iostream"

#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/idl.h>
#include <interface.h>

void PrintByteVect(const flatbuffers::Vector<uint8_t>* pVectData);

#endif