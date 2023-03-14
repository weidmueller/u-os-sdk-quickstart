// Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RESPONSETOACCESSORS_H_
#define RESPONSETOACCESSORS_H_

#include "iostream"

/**
 * @brief Show the content of a uc-slio-service response message.
 * 
 * @param [in] pRawResponseBuffer points to a uc-slio-service message buffer.
 * @returns -1 if an error occurred or 0 otherwise.
 * 
 * The function extracts the content from a uc-slio-service response message
 * with accessors. It prints to stdout the response type - specific content.
 * 
 */

int fbResponseToAccessors(uint8_t* pRawResponseBuffer);

#endif