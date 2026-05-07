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
 * Oklavier "guacurb" DVC plugin — Phase 1.1.1B-B.
 *
 * Provides the URBDRC ([MS-RDPEUSB]) listener that lets the Windows VM
 * negotiate a USB-redirection channel with us. We intentionally accept
 * the channel and log every PDU received but do NOT yet decode URB
 * frames or push USB IO traffic. That comes in 1.1.1B-C.
 *
 * Modelled on plugins/guacai/guacai.c (the audio-input DVC plugin) so
 * a future maintainer who already understands guacai will recognize
 * the lifecycle.
 */

#include "config.h"

#include "plugins/guacurb/guacurb.h"
#include "plugins/ptr-string.h"

#include <freerdp/dvc.h>
#include <freerdp/settings.h>
#include <guacamole/client.h>
#include <guacamole/mem.h>
#include <winpr/stream.h>
#include <winpr/wtsapi.h>
#include <winpr/wtypes.h>

#include <stdlib.h>

/**
 * Callback fired by FreeRDP whenever the URBDRC virtual channel receives
 * data from the Windows VM. In Phase 1.1.1B-C we'll dispatch on the
 * URBDRC PDU header (URB_COMPLETION, IOCONTROL_COMPLETION, etc.) and
 * forward results to the browser via guac_protocol_send_usbdata(). For
 * now we just log the size to confirm wiring.
 */
static UINT guac_rdp_urb_data(IWTSVirtualChannelCallback* channel_callback,
        wStream* stream) {

    guac_rdp_urb_channel_callback* cb =
            (guac_rdp_urb_channel_callback*) channel_callback;

    guac_client_log(cb->client, GUAC_LOG_TRACE,
            "[guacurb] URBDRC PDU received (%zu bytes) — dropped (1.1.1B-B)",
            (size_t) Stream_GetRemainingLength(stream));

    return CHANNEL_RC_OK;

}

/**
 * Callback fired when the URBDRC virtual channel is closed by the
 * server (e.g. session disconnect, RDP teardown). Frees the per-channel
 * state.
 */
static UINT guac_rdp_urb_close(IWTSVirtualChannelCallback* channel_callback) {

    guac_rdp_urb_channel_callback* cb =
            (guac_rdp_urb_channel_callback*) channel_callback;

    guac_client_log(cb->client, GUAC_LOG_DEBUG,
            "[guacurb] URBDRC channel closed");

    guac_mem_free(cb);
    return CHANNEL_RC_OK;

}

/**
 * Listener callback fired by FreeRDP when the Windows server initiates
 * the URBDRC channel. We accept and install the per-channel data/close
 * callbacks.
 *
 * The `accept` flag is read by FreeRDP after we return; it defaults to
 * TRUE (accept) and we leave it untouched. `data` is reserved per MS
 * docs.
 */
static UINT guac_rdp_urb_new_connection(
        IWTSListenerCallback* listener_callback, IWTSVirtualChannel* channel,
        BYTE* data, int* accept,
        IWTSVirtualChannelCallback** channel_callback) {

    guac_rdp_urb_listener_callback* listener =
            (guac_rdp_urb_listener_callback*) listener_callback;

    guac_client_log(listener->client, GUAC_LOG_DEBUG,
            "[guacurb] new URBDRC channel from server (accepting)");

    guac_rdp_urb_channel_callback* cb =
            guac_mem_zalloc(sizeof(guac_rdp_urb_channel_callback));
    cb->client = listener->client;
    cb->channel = channel;
    cb->parent.OnDataReceived = guac_rdp_urb_data;
    cb->parent.OnClose = guac_rdp_urb_close;

    *channel_callback = (IWTSVirtualChannelCallback*) cb;

    return CHANNEL_RC_OK;

}

/**
 * Plugin lifecycle: invoked once after RegisterPlugin() succeeds.
 * Allocates the listener and registers it for the "URBDRC" channel
 * name (per [MS-RDPEUSB] §2.1).
 */
static UINT guac_rdp_urb_initialize(IWTSPlugin* plugin,
        IWTSVirtualChannelManager* manager) {

    guac_rdp_urb_plugin* urb_plugin = (guac_rdp_urb_plugin*) plugin;

    guac_rdp_urb_listener_callback* listener_callback =
            guac_mem_zalloc(sizeof(guac_rdp_urb_listener_callback));
    urb_plugin->listener_callback = listener_callback;
    listener_callback->client = urb_plugin->client;
    listener_callback->parent.OnNewChannelConnection =
            guac_rdp_urb_new_connection;

    /* "URBDRC" is the channel name defined by [MS-RDPEUSB] for the
     * dynamic virtual channel that carries USB redirection traffic. */
    manager->CreateListener(manager, "URBDRC", 0,
            (IWTSListenerCallback*) listener_callback, NULL);

    return CHANNEL_RC_OK;

}

/**
 * Plugin lifecycle: invoked when drdynvc unloads us, e.g. on session
 * close. Frees plugin-owned memory. FreeRDP-owned objects (manager,
 * channels) must NOT be freed here.
 */
static UINT guac_rdp_urb_terminated(IWTSPlugin* plugin) {

    guac_rdp_urb_plugin* urb_plugin = (guac_rdp_urb_plugin*) plugin;
    guac_client* client = urb_plugin->client;

    guac_mem_free(urb_plugin->listener_callback);
    guac_mem_free(urb_plugin);

    guac_client_log(client, GUAC_LOG_DEBUG, "[guacurb] plugin unloaded");
    return CHANNEL_RC_OK;

}

/**
 * Entry point invoked by drdynvc when the "guacurb" plugin is requested.
 * Allocates the plugin object on first call, no-op on subsequent calls.
 *
 * Mirrors plugins/guacai/guacai.c::DVCPluginEntry — same handshake with
 * FreeRDP's plugin loader and same dlsym discovery path.
 */
UINT DVCPluginEntry(IDRDYNVC_ENTRY_POINTS* pEntryPoints) {

#ifdef PLUGIN_DATA_CONST
    const ADDIN_ARGV* args = pEntryPoints->GetPluginData(pEntryPoints);
#else
    ADDIN_ARGV* args = pEntryPoints->GetPluginData(pEntryPoints);
#endif

    /* The single argv we expect is the encoded guac_client pointer from
     * guac_rdp_ptr_to_string(client, &string) — same convention used by
     * the audio-input plugin. */
    guac_client* client = (guac_client*) guac_rdp_string_to_ptr(args->argv[1]);

    guac_rdp_urb_plugin* urb_plugin = (guac_rdp_urb_plugin*)
            pEntryPoints->GetPlugin(pEntryPoints, "guacurb");

    if (urb_plugin == NULL) {
        urb_plugin = guac_mem_zalloc(sizeof(guac_rdp_urb_plugin));
        urb_plugin->parent.Initialize = guac_rdp_urb_initialize;
        urb_plugin->parent.Terminated = guac_rdp_urb_terminated;
        urb_plugin->client = client;

        pEntryPoints->RegisterPlugin(pEntryPoints, "guacurb",
                (IWTSPlugin*) urb_plugin);

        guac_client_log(client, GUAC_LOG_DEBUG, "[guacurb] plugin loaded");
    }

    return CHANNEL_RC_OK;

}

