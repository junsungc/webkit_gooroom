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

WebInspector.WebSocketAccess = class WebSocketAccess extends WebInspector.Object
{
    constructor(id, url)
    {
        super();
        this._id = id;
        this._url = url;
        this._state = WebInspector.WebSocketAccess.State.CONNECTING;
        this._permission = WebInspector.WebSocketAccess.PermissionDecision.WAITING;
    }

    // Public

    get id()
    {
        return this._id;
    }

    get url()
    {
        return this._url;
    }

    set state(state)
    {
        this._state = state;
    }

    get state()
    {
        return this._state;
    }

    set permission(permission)
    {
        this._permission = permission;
    }

    get permission()
    {
        return this._permission;
    }
};

WebInspector.WebSocketAccess.PermissionDecision = {
    WAITING: "Waiting user's response",
    ALLOWED: "Allowed by user",
    REJECTED: "Rejected by user"
};

WebInspector.WebSocketAccess.State = {
    CONNECTING: "CONNECTING",
    OPEN: "OPEN",
    CLOSING: "CLOSING",
    CLOSED: "CLOSED"
};
