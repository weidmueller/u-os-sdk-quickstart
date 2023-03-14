// Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
//
// SPDX-License-Identifier: Apache-2.0

#include "iostream"

#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/idl.h>
#include <interface.h>

int JSONtoRequest(uint8_t* pRawRequestBuffer, std::string reqJSON){

    int buf_size = reqJSON.size() + 1;

    /* load schema file */
    std::string schemafile;
    bool ok = flatbuffers::LoadFile("/tmp/interface.fbs", false,
                                    &schemafile);
    if (!ok) {
      std::cout << "load file failed!" << std::endl;
      return -1;
    }

    /* create a Parser and let it digest the schema file */
    flatbuffers::Parser parser;
    parser.Parse(schemafile.c_str());

    /* parse the acutal JSON request */
    parser.Parse(reqJSON.c_str());

    /* structure the flatbuffer binary and the message size into the 
    uc-slio-service message buffer */
    auto buf_len = parser.builder_.GetSize();

    memcpy(pRawRequestBuffer+4, parser.builder_.GetBufferPointer(), buf_len);

    *((uint32_t*)pRawRequestBuffer) = buf_len+4;

    return(buf_len+4);
}
