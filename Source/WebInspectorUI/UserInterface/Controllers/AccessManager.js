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

WebInspector.AccessManager = class AccessManager extends WebInspector.Object
{
    constructor()
    {
        super();
        this.initialize();
    }

    // Public

    initialize()
    {
        this._workerAccesses = [];
        this._websocketAccesses = [];
        this._uiProcesses = [];
        this.createDummyProcesses();
    }

    // FIXME: This function create dummy processes. Need to replace with real processes data.
    createDummyProcesses()
    {
        this.uiProcesses.push(new WebInspector.UIProcess(1104, "master"));
        this.uiProcesses[this.uiProcesses.length - 1].addWebProcess(101, "www.mogaha.go.kr?...", true, true, true, true);
        this.uiProcesses[this.uiProcesses.length - 1].addWebProcess(101, "www.msip.go.kr", true, true, true, true);
        this.uiProcesses[this.uiProcesses.length - 1].addWebProcess(102, "www.google.co.kr?...", false, false, true, true);
        this.uiProcesses[this.uiProcesses.length - 1].addWebProcess(102, "www.naver.com", false, false, false, true);
        this.uiProcesses[this.uiProcesses.length - 1].addWebProcess(103, "www.webkit.org", false, false, false, true);
        this.uiProcesses.push(new WebInspector.UIProcess(2350, "commmon"));
        this.uiProcesses[this.uiProcesses.length - 1].addWebProcess(2351, "www.google.co.kr", false, false, false, false);
        this.dispatchEventToListeners(WebInspector.AccessManager.Event.ProcessDummyUpdated);
    }

    workerDidSendPermissionRequest(id, scriptURL)
    {
        this.workerAccesses.push(new WebInspector.WorkerAccess(id, scriptURL));
        this.dispatchEventToListeners(WebInspector.AccessManager.Event.WorkersUpdated);
    }

    workerDidReceivePermissionResponse(id, allowed)
    {
        for (var workerAccess of this._workerAccesses) {
            if (workerAccess.id === id) {
                workerAccess.permission = allowed ? WebInspector.WorkerAccess.PermissionDecision.ALLOWED : WebInspector.WorkerAccess.PermissionDecision.REJECTED;
            this.dispatchEventToListeners(WebInspector.AccessManager.Event.WorkersUpdated);
            break;
            }
        }
    }

    websocketDidSendPermissionRequest(id, url)
    {
        this.websocketAccesses.push(new WebInspector.WebSocketAccess(id, url));
        this.dispatchEventToListeners(WebInspector.AccessManager.Event.WebSocketsUpdated);
    }

    websocketDidReceivePermissionResponse(id, allowed)
    {
        for (var websocketAccess of this._websocketAccesses) {
            if (websocketAccess.id === id) {
                websocketAccess.permission = allowed ? WebInspector.WebSocketAccess.PermissionDecision.ALLOWED : WebInspector.WebSocketAccess.PermissionDecision.REJECTED;
                websocketAccess.state = allowed ? WebInspector.WebSocketAccess.State.OPEN : WebInspector.WebSocketAccess.State.CLOSED;
                this.dispatchEventToListeners(WebInspector.AccessManager.Event.WebSocketsUpdated);
                break;
            }
        }
    }

    websocketDidClose(id)
    {
        for (var websocketAccess of this._websocketAccesses) {
            if (websocketAccess.id === id) {
                websocketAccess.state = WebInspector.WebSocketAccess.State.CLOSED;
                this.dispatchEventToListeners(WebInspector.AccessManager.Event.WebSocketsUpdated);
                break;
            }
        }
    }

    get workerAccesses()
    {
        return this._workerAccesses;
    }

    get websocketAccesses()
    {
        return this._websocketAccesses;
    }

    get uiProcesses()
    {
        return this._uiProcesses;
    }
};

WebInspector.AccessManager.Event = {
    ProcessDummyUpdated: "accessmanager-process-dummy-updated",
    WorkersUpdated: "accessmanager-workers-updated",
    WebSocketsUpdated: "accessmanager-websockets-updated"
};
