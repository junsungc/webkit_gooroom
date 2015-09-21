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

#if !defined(__WEBKIT2_H_INSIDE__) && !defined(WEBKIT2_COMPILATION)
#error "Only <webkit2/webkit2.h> can be included directly."
#endif

#ifndef WebKitWebSocketPermissionRequest_h
#define WebKitWebSocketPermissionRequest_h

#include <glib-object.h>
#include <webkit2/WebKitDefines.h>

G_BEGIN_DECLS

#define WEBKIT_TYPE_WEBSOCKET_PERMISSION_REQUEST            (webkit_websocket_permission_request_get_type())
#define WEBKIT_WEBSOCKET_PERMISSION_REQUEST(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), WEBKIT_TYPE_WEBSOCKET_PERMISSION_REQUEST, WebKitWebSocketPermissionRequest))
#define WEBKIT_WEBSOCKET_PERMISSION_REQUEST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  WEBKIT_TYPE_WEBSOCKET_PERMISSION_REQUEST, WebKitWebSocketPermissionRequestClass))
#define WEBKIT_IS_WEBSOCKET_PERMISSION_REQUEST(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), WEBKIT_TYPE_WEBSOCKET_PERMISSION_REQUEST))
#define WEBKIT_IS_WEBSOCKET_PERMISSION_REQUEST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  WEBKIT_TYPE_WEBSOCKET_PERMISSION_REQUEST))
#define WEBKIT_WEBSOCKET_PERMISSION_REQUEST_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  WEBKIT_TYPE_WEBSOCKET_PERMISSION_REQUEST, WebKitWebSocketPermissionRequestClass))

typedef struct _WebKitWebSocketPermissionRequest        WebKitWebSocketPermissionRequest;
typedef struct _WebKitWebSocketPermissionRequestClass   WebKitWebSocketPermissionRequestClass;
typedef struct _WebKitWebSocketPermissionRequestPrivate WebKitWebSocketPermissionRequestPrivate;

struct _WebKitWebSocketPermissionRequest {
    GObject parent;

    /*< private >*/
    WebKitWebSocketPermissionRequestPrivate *priv;
};

struct _WebKitWebSocketPermissionRequestClass {
    GObjectClass parent_class;

    void (*_webkit_reserved0) (void);
    void (*_webkit_reserved1) (void);
    void (*_webkit_reserved2) (void);
    void (*_webkit_reserved3) (void);
};

WEBKIT_API GType
webkit_websocket_permission_request_get_type (void);

G_END_DECLS

#endif
