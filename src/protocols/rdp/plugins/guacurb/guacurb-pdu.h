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

/*
 * URBDRC PDU constants per [MS-RDPEUSB]. Mirrors the constants used by
 * FreeRDP 3.x's channels/urbdrc/client/data_transfer.{c,h} so a future
 * maintainer can cross-reference the FreeRDP implementation when the
 * MS spec is ambiguous.
 *
 * Phase 1.1.1B-C-1 — these are read-only by guacd today; we use them to
 * decode the PDU header and log what Windows sends us. Encoding our own
 * responses lands in 1.1.1B-C-2 once we have observed traffic.
 */

#ifndef GUAC_RDP_PLUGINS_GUACURB_PDU_H
#define GUAC_RDP_PLUGINS_GUACURB_PDU_H

#include <winpr/wtypes.h>

/*
 * PDU header layout (12 bytes, all UINT32 little-endian on the wire):
 *
 *     +0  InterfaceID  (high 2 bits = stream type, low 30 = interface)
 *     +4  MessageID    (request/response correlation)
 *     +8  FunctionID   (operation; meaning depends on InterfaceID)
 *
 * After that comes a variable payload. See [MS-RDPEUSB] §2.2.
 */

/**
 * Top-2-bit stream type encoded into the InterfaceID. PROXY identifies
 * traffic addressed to a per-device proxy (active USB device), STUB is
 * the unattached device stub, the unmasked low 30 bits are the actual
 * interface or device handle.
 */
#define GUAC_URB_STREAM_ID_NONE   0x0
#define GUAC_URB_STREAM_ID_STUB   0x1
#define GUAC_URB_STREAM_ID_PROXY  0x2

#define GUAC_URB_STREAM_ID(id)    (((id) >> 30) & 0x3)
#define GUAC_URB_INTERFACE(id)    ((id) & 0x3FFFFFFFu)

/*
 * Well-known interface IDs ([MS-RDPEUSB] §2.2.4 and §2.2.5):
 *
 * The "capabilities" interface is what the server uses to ask us about
 * version + extended caps. The "server channel" interface (also
 * sometimes called "channel notification") is how the server tells us
 * to add/remove devices. Per-device interfaces sit on top of those at
 * IDs allocated dynamically.
 */
#define GUAC_URB_IF_CAPABILITIES        0x00000001u
#define GUAC_URB_IF_SERVER_CHANNEL      0x00000002u
#define GUAC_URB_IF_CHANNEL_NOTIFICATION 0x00000003u

/*
 * Function IDs on the CAPABILITIES interface ([MS-RDPEUSB] §2.2.4).
 */
#define GUAC_URB_FN_RIM_EXCHANGE_CAPABILITY_REQUEST  0x00000000u

/*
 * Function IDs on the SERVER_CHANNEL / CHANNEL_NOTIFICATION interfaces
 * ([MS-RDPEUSB] §2.2.5). The server uses these to tell us when it
 * accepted (or rejected) a device we added.
 */
#define GUAC_URB_FN_CHANNEL_CREATED                  0x00000001u
#define GUAC_URB_FN_QUERY_DEVICE_TEXT                0x00000002u

/*
 * Function IDs on a per-device interface ([MS-RDPEUSB] §2.2.6 onwards).
 * Server-to-client (we receive); client-to-server completions reuse
 * these IDs +0x80000000 in Microsoft docs but FreeRDP encodes the
 * direction differently — see urbdrc/client/data_transfer.c.
 */
#define GUAC_URB_FN_CANCEL_REQUEST                   0x00000020u
#define GUAC_URB_FN_REGISTER_REQUEST_CALLBACK        0x00000021u
#define GUAC_URB_FN_IO_CONTROL                       0x00000022u
#define GUAC_URB_FN_INTERNAL_IO_CONTROL              0x00000023u
#define GUAC_URB_FN_QUERY_DEVICE_TEXT_REQUEST        0x00000024u
#define GUAC_URB_FN_TRANSFER_IN_REQUEST              0x00000025u
#define GUAC_URB_FN_TRANSFER_OUT_REQUEST             0x00000026u
#define GUAC_URB_FN_RETRACT_DEVICE                   0x00000027u

/*
 * Completion FunctionIDs we'll send back ([MS-RDPEUSB] §2.2.7).
 */
#define GUAC_URB_FN_URB_COMPLETION                   0x00000064u
#define GUAC_URB_FN_URB_COMPLETION_NO_DATA           0x00000065u
#define GUAC_URB_FN_IO_CONTROL_COMPLETION            0x00000066u

/**
 * Friendly name for a FunctionID — used by the trace logger so a future
 * maintainer reading guacd logs can tell at a glance what kind of PDU
 * Windows just sent. Returns "?" for unknown values.
 */
const char* guac_rdp_urb_function_name(UINT32 interface_id,
        UINT32 function_id);

#endif

