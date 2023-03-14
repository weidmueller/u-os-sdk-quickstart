// Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
//
// SPDX-License-Identifier: Apache-2.0

#ifndef PREPAREREQUESTFROMJSON_H_
#define PREPAREREQUESTFROMJSON_H_

/**
 * @brief Parse a JSON request and return a uc-slio-service message.
 * 
 * @param [in] reqJSON a request in JSON
 * @param [out] pRawRequestBuffer points to a uc-slio-service message buffer. Reserve enough memory, ahead.
 * @returns the actual size of the uc-slio-service-message.
 * 
 * The function parses the JSON according to the flatbuffer schema in 
 * /tmp/interface.fbs. Then it structures the flatbuffer binary and the size of the
 * message into the raw request buffer.
 * 
 * The actual messages transceived to/from the uc-slio-service also contain the 
 * message size:
 * 
<table>
<caption id="multi_row">message layout</caption>
<tr><th>Position <th>Content
<tr><td>0-3 <td>uint32_t message length
<tr><td>4-end <td>uint8_t* flatbuffers binary
</table>
 */

    int JSONtoRequest(uint8_t* pRawRequestBuffer, std::string reqJSON);

#endif