/*
* Copyright (C) 2015 Company 100, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "config.h"
#include "WebKitWebSocketPermissionRequest.h"

#include "WebKitPermissionRequest.h"
#include "WebKitWebSocketPermissionRequestPrivate.h"
#include "WebSocketPermissionRequestProxy.h"

using namespace WebKit;

static void webkit_permission_request_interface_init(WebKitPermissionRequestIface*);

struct _WebKitWebSocketPermissionRequestPrivate {
    RefPtr<WebSocketPermissionRequestProxy> request;
    bool madeDecision;
};

WEBKIT_DEFINE_TYPE_WITH_CODE(
    WebKitWebSocketPermissionRequest, webkit_websocket_permission_request, G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE(WEBKIT_TYPE_PERMISSION_REQUEST, webkit_permission_request_interface_init))

static void webkitWebSocketPermissionRequestAllow(WebKitPermissionRequest* request)
{
    ASSERT(WEBKIT_IS_WEBSOCKET_PERMISSION_REQUEST(request));

    WebKitWebSocketPermissionRequestPrivate* priv = WEBKIT_WEBSOCKET_PERMISSION_REQUEST(request)->priv;

    if (priv->madeDecision)
        return;

    priv->request->allow();
    priv->madeDecision = true;
}

static void webkitWebSocketPermissionRequestDeny(WebKitPermissionRequest* request)
{
    ASSERT(WEBKIT_IS_WEBSOCKET_PERMISSION_REQUEST(request));

    WebKitWebSocketPermissionRequestPrivate* priv = WEBKIT_WEBSOCKET_PERMISSION_REQUEST(request)->priv;

    if (priv->madeDecision)
        return;

    priv->request->deny();
    priv->madeDecision = true;
}

static void webkit_permission_request_interface_init(WebKitPermissionRequestIface* iface)
{
    iface->allow = webkitWebSocketPermissionRequestAllow;
    iface->deny = webkitWebSocketPermissionRequestDeny;
}

static void webkitWebSocketPermissionRequestDispose(GObject* object)
{
    webkitWebSocketPermissionRequestDeny(WEBKIT_PERMISSION_REQUEST(object));
    G_OBJECT_CLASS(webkit_websocket_permission_request_parent_class)->dispose(object);
}

static void webkit_websocket_permission_request_class_init(WebKitWebSocketPermissionRequestClass* klass)
{
    GObjectClass* objectClass = G_OBJECT_CLASS(klass);
    objectClass->dispose = webkitWebSocketPermissionRequestDispose;
}

WebKitWebSocketPermissionRequest* webkitWebSocketPermissionRequestCreate(WebSocketPermissionRequestProxy* request)
{
    WebKitWebSocketPermissionRequest* websocketPermissionRequest = WEBKIT_WEBSOCKET_PERMISSION_REQUEST(g_object_new(WEBKIT_TYPE_WEBSOCKET_PERMISSION_REQUEST, NULL));
    websocketPermissionRequest->priv->request = request;
    return websocketPermissionRequest;
}
