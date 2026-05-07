/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "config.h"

#include "plugins/guacurb/guacurb-pdu.h"

#include <winpr/wtypes.h>

const char* guac_rdp_urb_function_name(UINT32 interface_id,
        UINT32 function_id) {

    switch (interface_id) {

        case GUAC_URB_IF_CAPABILITIES:
            switch (function_id) {
                case GUAC_URB_FN_RIM_EXCHANGE_CAPABILITY_REQUEST:
                    return "RIM_EXCHANGE_CAPABILITY_REQUEST";
            }
            return "capabilities/?";

        case GUAC_URB_IF_SERVER_CHANNEL:
        case GUAC_URB_IF_CHANNEL_NOTIFICATION:
            switch (function_id) {
                case GUAC_URB_FN_CHANNEL_CREATED:    return "CHANNEL_CREATED";
                case GUAC_URB_FN_QUERY_DEVICE_TEXT:  return "QUERY_DEVICE_TEXT";
            }
            return "server-channel/?";

        default:
            /* Per-device interfaces — function IDs are well-known. */
            switch (function_id) {
                case GUAC_URB_FN_CANCEL_REQUEST:
                    return "CANCEL_REQUEST";
                case GUAC_URB_FN_REGISTER_REQUEST_CALLBACK:
                    return "REGISTER_REQUEST_CALLBACK";
                case GUAC_URB_FN_IO_CONTROL:
                    return "IO_CONTROL";
                case GUAC_URB_FN_INTERNAL_IO_CONTROL:
                    return "INTERNAL_IO_CONTROL";
                case GUAC_URB_FN_QUERY_DEVICE_TEXT_REQUEST:
                    return "QUERY_DEVICE_TEXT_REQUEST";
                case GUAC_URB_FN_TRANSFER_IN_REQUEST:
                    return "TRANSFER_IN_REQUEST";
                case GUAC_URB_FN_TRANSFER_OUT_REQUEST:
                    return "TRANSFER_OUT_REQUEST";
                case GUAC_URB_FN_RETRACT_DEVICE:
                    return "RETRACT_DEVICE";
                case GUAC_URB_FN_URB_COMPLETION:
                    return "URB_COMPLETION";
                case GUAC_URB_FN_URB_COMPLETION_NO_DATA:
                    return "URB_COMPLETION_NO_DATA";
                case GUAC_URB_FN_IO_CONTROL_COMPLETION:
                    return "IO_CONTROL_COMPLETION";
            }
            return "device/?";
    }

}

