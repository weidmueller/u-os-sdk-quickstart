// Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RESPONSETOJSON_H_
#define RESPONSETOJSON_H_

#include "iostream"

/**
 * @brief Parses a uc-slio-service response message to JSON and prints it.
 *
 * @param [in] pRawResponseBuffer points to a uc-slio-service message.
 * 
 * @returns -1 if an error occured; 0 otherwise.
 * 
 * Checks the uc-slio-service response message for validity. Reads a flatbuffer
 * schema from /tmp/interface.fbs and passes it to a parser. Parses the 
 * uc-slio-service message payload to JSON and prints it to stdout.
 */

    int fbResponsetoJSON(uint8_t* pRawResponseBuffer);

#endif