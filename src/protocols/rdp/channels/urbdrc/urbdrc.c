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
 * Oklavier URBDRC bridge — Phase 1.1.1B-A skeleton.
 *
 * Takes Guacamole-protocol USB instructions (usbconnect / usbdata /
 * usbdisconnect) and *will eventually* forward URBs to a Windows VM
 * over the URBDRC dynamic virtual channel. For now, the handlers are
 * wired and log so we can confirm:
 *   1. The build links cleanly with the libguac scaffolding from PR #610.
 *   2. The instructions arrive on guacd as expected.
 *   3. settings->enable_usb_redirect gates everything behind a config
 *      flag (so this remains a no-op until our viewer opts in).
 *
 * The actual DVC plugin and URB framing land in 1.1.1B-B and 1.1.1B-C.
 */

#include "channels/urbdrc/urbdrc.h"
#include "rdp.h"

#include <freerdp/freerdp.h>
#include <guacamole/client.h>
#include <guacamole/user.h>

#include <string.h>

int guac_rdp_usbconnect_handler(guac_user* user,
        const char* device_id, int vendor_id, int product_id,
        const char* device_name, const char* serial_number,
        int device_class, int device_subclass, int device_protocol,
        const char* interface_data) {

    guac_client_log(user->client, GUAC_LOG_DEBUG,
            "[urbdrc] usbconnect device=%s vid=%04x pid=%04x class=%d:%d:%d "
            "name=\"%s\" serial=\"%s\" interfaces=\"%s\" — Phase 1.1.1B-A no-op",
            device_id ? device_id : "(null)",
            vendor_id, product_id,
            device_class, device_subclass, device_protocol,
            device_name   ? device_name   : "",
            serial_number ? serial_number : "",
            interface_data ? interface_data : "");

    /* Phase 1.1.1B-B: allocate per-device state, register endpoints with
     * the guacurb DVC, send IOCONTROL_URB_DEVICE_INSERT to the VM. */
    return 0;
}

int guac_rdp_usbdata_handler(guac_user* user,
        const char* device_id, int endpoint_number,
        const char* data, const char* transfer_type) {

    guac_client_log(user->client, GUAC_LOG_TRACE,
            "[urbdrc] usbdata device=%s ep=%d type=%s len(b64)=%zu — drop",
            device_id ? device_id : "(null)",
            endpoint_number,
            transfer_type ? transfer_type : "",
            data ? strlen(data) : (size_t) 0);

    /* Phase 1.1.1B-C: base64-decode `data`, build a URB_REQUEST PDU, push
     * to the DVC. Responses (server-to-client) flow back through
     * guac_protocol_send_usbdata from the DVC callback. */
    return 0;
}

int guac_rdp_usbdisconnect_handler(guac_user* user, const char* device_id) {

    guac_client_log(user->client, GUAC_LOG_DEBUG,
            "[urbdrc] usbdisconnect device=%s — Phase 1.1.1B-A no-op",
            device_id ? device_id : "(null)");

    /* Phase 1.1.1B-B: send IOCONTROL_URB_DEVICE_REMOVE on the DVC and free
     * per-device state. */
    return 0;
}

void guac_rdp_urbdrc_load_plugin(rdpContext* context) {

    guac_client* client = ((rdp_freerdp_context*) context)->client;
    guac_client_log(client, GUAC_LOG_DEBUG,
            "[urbdrc] enable_usb_redirect=true — Phase 1.1.1B-A: handlers "
            "wired, DVC plugin not yet registered");

    /* Phase 1.1.1B-B: register the "guacurb" DVC plugin via
     * guac_freerdp_dynamic_channel_collection_add or by invoking
     * drdynvc's plugin entry point with our DVCPluginEntry callback. */
}

