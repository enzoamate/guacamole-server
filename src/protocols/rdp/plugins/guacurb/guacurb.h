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
 * Oklavier downstream FreeRDP DVC plugin — "guacurb".
 *
 * Loaded by drdynvc when the Oklavier-extended guacd registers it via
 * guac_freerdp_dynamic_channel_collection_add(settings, "guacurb", ...).
 * The plugin opens a listener for the URBDRC dynamic virtual channel
 * defined by [MS-RDPEUSB] and is the bridge endpoint that the
 * channels/urbdrc/ side of guacd talks to.
 *
 * Phase 1.1.1B-B — the plugin is present, listener is registered, and
 * connection events are logged. Actual URB/IO_REQUEST framing lands in
 * Phase 1.1.1B-C.
 */

#ifndef GUAC_RDP_PLUGINS_GUACURB_H
#define GUAC_RDP_PLUGINS_GUACURB_H

#include <freerdp/constants.h>
#include <freerdp/dvc.h>
#include <freerdp/freerdp.h>
#include <guacamole/client.h>

/**
 * Extended IWTSListenerCallback giving us access to the guac_client. Mirrors
 * the guacai pattern; the `parent` field MUST be first so FreeRDP can cast
 * the pointer back to IWTSListenerCallback.
 */
typedef struct guac_rdp_urb_listener_callback {

    /**
     * The parent IWTSListenerCallback structure. MUST be first.
     */
    IWTSListenerCallback parent;

    /**
     * The Guacamole client this listener belongs to.
     */
    guac_client* client;

} guac_rdp_urb_listener_callback;

/**
 * Extended IWTSVirtualChannelCallback for an active URBDRC channel. The
 * parent MUST be first so FreeRDP's downcasts work.
 */
typedef struct guac_rdp_urb_channel_callback {

    /**
     * The parent IWTSVirtualChannelCallback structure. MUST be first.
     */
    IWTSVirtualChannelCallback parent;

    /**
     * The URBDRC virtual channel through which we send PDUs back to the
     * Windows VM. Owned by FreeRDP — do not free.
     */
    IWTSVirtualChannel* channel;

    /**
     * The Guacamole client this channel belongs to.
     */
    guac_client* client;

} guac_rdp_urb_channel_callback;

/**
 * Top-level DVC plugin struct. Allocated once per RDP session and kept
 * alive for the lifetime of the connection.
 */
typedef struct guac_rdp_urb_plugin {

    /**
     * The parent IWTSPlugin structure. MUST be first.
     */
    IWTSPlugin parent;

    /**
     * The listener callback allocated at Initialize() time. Freed in
     * Terminated().
     */
    guac_rdp_urb_listener_callback* listener_callback;

    /**
     * The Guacamole client this plugin belongs to.
     */
    guac_client* client;

} guac_rdp_urb_plugin;

#endif

