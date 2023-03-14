// Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
//
// SPDX-License-Identifier: Apache-2.0

#include "iostream"

#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/idl.h>
#include <interface.h>

void PrintByteVect(const flatbuffers::Vector<uint8_t>* pVectData){
    std::cout << std::hex << std::setfill('0');  // needs to be set only once

    using u64 = unsigned long long;

    for (int i = 0; i < pVectData->size(); i++) {
        if (i % sizeof(u64) == 0) {
        std::cout << std::endl;
        std::cout << std::setw(2) << i << ": ";
        }
        std::cout << std::setw(2) << static_cast<unsigned>((*pVectData)[i]) << " ";
    }
    std::cout << std::endl;
}