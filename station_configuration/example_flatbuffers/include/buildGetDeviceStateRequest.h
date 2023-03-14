// Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
//
// SPDX-License-Identifier: Apache-2.0

#ifndef BUILDGETDEVICESTATEREQUEST_H_
#define BUILDGETDEVICESTATEREQUEST_H_

/**
 * @brief Serializes a get device state request with flatbuffers builder methods.
 * 
 * We serialize the following request bottom-up:
 * 
 * {
 *  message_type: "Request",
 *  message: 
 *    {
 *      request_type: "GetDeviceStateRequest",
 *      request: 
 *        {
 *          name: "slio/3"
 *        }
 *    }
 * }
 * 
 * The uc-slio-service api makes frequent use of flatbuffers unions. Please
 * refer to the FlatBuffer Schema Tutorial for related information. Also refer
 * to api/interface.fbs for the uc-slio-service's FlatBuffer schema.
 * 
 * We add a device name to a GetDeviceStateRequest and serialize it.
 * 
 * We show how to add type and content to the AnyRequest- and AnyMessage unions
 * and serialize them.
 * 
 * We show how to serialize the message from the flatbuffer builder into a 
 * uint8_t* buffer.
 * 
 * We show how to structure the flatbuffer binary and its length into a
 * uc-slio-service socket message.
 * 
 */

    void buildGetDeviceStateRequest(uint8_t* p_raw_request_buffer);

#endif