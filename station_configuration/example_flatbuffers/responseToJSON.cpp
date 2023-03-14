// Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
//
// SPDX-License-Identifier: Apache-2.0

#include "iostream"

#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/idl.h>
#include <interface.h>

using namespace std;
using namespace weidmueller::ucontrol::devices;

int fbResponsetoJSON(uint8_t* pRawResponseBuffer){

    /* get the message size, check size and message payload */
    uint32_t* pRawSize = (uint32_t*)pRawResponseBuffer;

    if (pRawSize == 0){
        cout << "Error: response buffer is empty." << endl;
        return -1;
    }
    auto myVeri = flatbuffers::Verifier((uint8_t*)(pRawResponseBuffer+4), *pRawSize-4);

    if(!VerifyMessageBuffer(myVeri)) {
        cout << "Error: response buffer contains invalid message." << endl;
        return -1;
    };

    /* Read flatbuffer schema from file and digest it into a Parser */
    std::string schemafile;
    bool ok = flatbuffers::LoadFile("/tmp/interface.fbs", false,
                                    &schemafile);
    if (!ok) {
      std::cout << "load file failed!" << std::endl;
      return -1;
    }

    flatbuffers::Parser response_parser;
    response_parser.Parse(schemafile.c_str());

    /* Generate JSON from uc-slio-service message payload and print it to stdout */
    string jsongen;
    jsongen.clear();

    if (!GenerateText(response_parser, pRawResponseBuffer+4, &jsongen)) {
      std::cout << "Couldn't serialize parsed data to JSON!" << std::endl;
      return -1;
    } else
        cout << jsongen << endl;

    return 0;
}