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
 * Oklavier downstream extension. Bridges Guacamole-protocol USB
 * instructions (usbconnect / usbdata / usbdisconnect, added by upstream
 * PR #610 / GUACAMOLE-522) to the FreeRDP URBDRC dynamic virtual channel,
 * so a browser using WebUSB can forward a USB device into the remote
 * Windows session.
 *
 * Phase 1.1.1B-A — skeleton. Handlers are wired and log; no URB traffic
 * is forwarded yet. The DVC plugin glue lands in 1.1.1B-B.
 */

#ifndef GUAC_RDP_CHANNELS_URBDRC_H
#define GUAC_RDP_CHANNELS_URBDRC_H

#include <freerdp/freerdp.h>
#include <guacamole/user.h>

/**
 * Handler for "usbconnect" Guacamole instructions arriving from the user
 * (browser). Called by libguac after the instruction is parsed.
 *
 * Stores the device descriptor on the RDP client so subsequent usbdata
 * traffic can be routed to the right URBDRC endpoint. Phase 1.1.1B-A
 * just logs and acknowledges; no FreeRDP integration yet.
 */
guac_user_usbconnect_handler guac_rdp_usbconnect_handler;

/**
 * Handler for "usbdata" Guacamole instructions arriving from the user.
 * Each instruction is one USB transfer (control / bulk / interrupt /
 * isochronous) targeted at a specific endpoint of an already-connected
 * device.
 *
 * Phase 1.1.1B-A logs and discards; in 1.1.1B-C this will be encoded as a
 * URBDRC URB_REQUEST and pushed to the DVC.
 */
guac_user_usbdata_handler guac_rdp_usbdata_handler;

/**
 * Handler for "usbdisconnect" Guacamole instructions. Tears down the
 * URBDRC virtual device on the remote Windows side and frees the
 * client-side bookkeeping for the device.
 */
guac_user_usbdisconnect_handler guac_rdp_usbdisconnect_handler;

/**
 * Loads the Oklavier "guacurb" dynamic virtual channel plugin into
 * FreeRDP's drdynvc, which is what carries URBDRC frames between guacd
 * and the Windows VM. Must be called only when
 * settings->enable_usb_redirect is true and after drdynvc itself is
 * loaded.
 *
 * Phase 1.1.1B-A: this is a no-op (logs and returns). The full DVC
 * plugin lands in the next iteration.
 *
 * @param context
 *     The rdpContext associated with the active RDP session.
 */
void guac_rdp_urbdrc_load_plugin(rdpContext* context);

#endif

