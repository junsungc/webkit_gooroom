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
#include "WebSocketController.h"

#if ENABLE(WEB_SOCKETS)

#include "WebSocketClient.h"

namespace WebCore {

WebSocketController::WebSocketController(Page& page, WebSocketClient& client)
    : m_page(page)
    , m_client(client)
{
}

WebSocketController::~WebSocketController()
{
    // NOTE: We don't have to remove ourselves from page's ViewStateChangeObserver set, since
    // we are supplement of the Page, and our destructor getting called means the page is being
    // torn down.

    m_client.websocketDestroyed();
}

void WebSocketController::requestPermission(WebSocket* websocket)
{
    if (!m_page.isVisible()) {
        m_pendedPermissionRequest.add(websocket);
        return;
    }

    m_client.requestPermission(websocket);
}

void WebSocketController::cancelPermissionRequest(WebSocket* websocket)
{
    if (m_pendedPermissionRequest.remove(websocket))
        return;

    m_client.cancelPermissionRequest(websocket);
}

const char* WebSocketController::supplementName()
{
    return "WebSocketController";
}

void provideWebSocketTo(Page* page, WebSocketClient* client)
{
    ASSERT(page);
    ASSERT(client);
    Supplement<Page>::provideTo(page, WebSocketController::supplementName(), std::make_unique<WebSocketController>(*page, *client));
}

} // namespace WebCore

#endif // ENABLE(WEB_SOCKETS)
