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

#ifndef WebSocketController_h
#define WebSocketController_h

#if ENABLE(WEB_SOCKETS)

#include "Page.h"
#include "WebSocket.h"
#include <wtf/HashSet.h>
#include <wtf/Noncopyable.h>
#include <wtf/RefPtr.h>

namespace WebCore {

class WebSocketClient;
class WebSocketError;
class Page;

class WebSocketController : public Supplement<Page> {
    WTF_MAKE_FAST_ALLOCATED;
    WTF_MAKE_NONCOPYABLE(WebSocketController);
public:
    WebSocketController(Page&, WebSocketClient&);
    ~WebSocketController();

    void requestPermission(WebSocket*);
    void cancelPermissionRequest(WebSocket*);

    void receivePermissionDecision(WebSocket*, bool allowed);

    WebSocketClient& client() { return m_client; }

    WEBCORE_EXPORT static const char* supplementName();
    static WebSocketController* from(Page* page) { return static_cast<WebSocketController*>(Supplement<Page>::from(page, supplementName())); }

private:
    Page& m_page;
    WebSocketClient& m_client;

    // While the page is not visible, we pend permission requests.
    HashSet<RefPtr<WebSocket>> m_pendedPermissionRequest;
};

} // namespace WebCore

#endif // ENABLE(WEB_SOCKETS)

#endif // WebSocketController_h
