/*
 * Copyright (C) 2015 Company 100, Inc.
 * Copyright (C) 2012 Igalia S.L.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "WebKitWorkerPermissionRequest.h"

#include "WebKitPermissionRequest.h"
#include "WebKitWorkerPermissionRequestPrivate.h"
#include "WorkerPermissionRequestProxy.h"

using namespace WebKit;

/**
 * SECTION: WebKitWorkerPermissionRequest
 * @Short_description: A permission request for creating Worker.
 * @Title: WebKitWorkerPermissionRequest
 * @See_also: #WebKitPermissionRequest, #WebKitWebView
 *
 * WebKitWorkerPermissionRequest represents a request for
 * permission to decide whether WebKit should provide worker to
 * a website when requested throught the Worker API.
 *
 * When a WebKitWorkerPermissionRequest is not handled by the user,
 * it is denied by default.
 */

static void webkit_permission_request_interface_init(WebKitPermissionRequestIface*);

struct _WebKitWorkerPermissionRequestPrivate {
    RefPtr<WorkerPermissionRequestProxy> request;
    bool madeDecision;
};

WEBKIT_DEFINE_TYPE_WITH_CODE(
    WebKitWorkerPermissionRequest, webkit_worker_permission_request, G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE(WEBKIT_TYPE_PERMISSION_REQUEST, webkit_permission_request_interface_init))

static void webkitWorkerPermissionRequestAllow(WebKitPermissionRequest* request)
{
    ASSERT(WEBKIT_IS_WORKER_PERMISSION_REQUEST(request));

    WebKitWorkerPermissionRequestPrivate* priv = WEBKIT_WORKER_PERMISSION_REQUEST(request)->priv;

    // Only one decision at a time.
    if (priv->madeDecision)
        return;

    priv->request->allow();
    priv->madeDecision = true;
}

static void webkitWorkerPermissionRequestDeny(WebKitPermissionRequest* request)
{
    ASSERT(WEBKIT_IS_WORKER_PERMISSION_REQUEST(request));

    WebKitWorkerPermissionRequestPrivate* priv = WEBKIT_WORKER_PERMISSION_REQUEST(request)->priv;

    // Only one decision at a time.
    if (priv->madeDecision)
        return;

    priv->request->deny();
    priv->madeDecision = true;
}

static void webkit_permission_request_interface_init(WebKitPermissionRequestIface* iface)
{
    iface->allow = webkitWorkerPermissionRequestAllow;
    iface->deny = webkitWorkerPermissionRequestDeny;
}

static void webkitWorkerPermissionRequestDispose(GObject* object)
{
    // Default behaviour when no decision has been made is denying the request.
    webkitWorkerPermissionRequestDeny(WEBKIT_PERMISSION_REQUEST(object));
    G_OBJECT_CLASS(webkit_worker_permission_request_parent_class)->dispose(object);
}

static void webkit_worker_permission_request_class_init(WebKitWorkerPermissionRequestClass* klass)
{
    GObjectClass* objectClass = G_OBJECT_CLASS(klass);
    objectClass->dispose = webkitWorkerPermissionRequestDispose;
}

WebKitWorkerPermissionRequest* webkitWorkerPermissionRequestCreate(WorkerPermissionRequestProxy* request)
{
    WebKitWorkerPermissionRequest* workerPermissionRequest = WEBKIT_WORKER_PERMISSION_REQUEST(g_object_new(WEBKIT_TYPE_WORKER_PERMISSION_REQUEST, NULL));
    workerPermissionRequest->priv->request = request;
    return workerPermissionRequest;
}
