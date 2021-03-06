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
#include "WebSocketPermissionRequestManager.h"

#if ENABLE(WEB_SOCKETS)

#include "WebCoreArgumentCoders.h"
#include "WebFrame.h"
#include "WebPage.h"
#include "WebPageProxyMessages.h"
#include <WebCore/Document.h>
#include <WebCore/Frame.h>
#include <WebCore/FrameLoader.h>
#include <WebCore/SecurityOrigin.h>
#include <WebCore/WebSocket.h>
#include <WebCore/WebSocketController.h>


#include "Page.h"

using namespace WebCore;

namespace WebKit {

static uint64_t generateWebSocketID()
{
    static uint64_t uniqueWebSocketID = 123;
    uniqueWebSocketID+=5;
    return uniqueWebSocketID;
}

WebSocketPermissionRequestManager::WebSocketPermissionRequestManager(WebPage* page)
    : m_page(page)
{
    printf("Iam requestManager\n");
}

void WebSocketPermissionRequestManager::startRequestForWebSocket(WebSocket* websocket)
{
    Frame* frame = websocket->frame();

    ASSERT_WITH_MESSAGE(frame, "It is not well understood in which cases the WebSocket is alive after its frame goes away. If you hit this assertion, please add a test covering this case.");
    if (!frame)
        return;

    uint64_t websocketID = generateWebSocketID();

    m_websocketToIDMap.set(websocket, websocketID);
    m_idToWebSocketMap.set(websocketID, websocket);
    printf("str websocket : %p\n", websocket);

    WebFrame* webFrame = WebFrame::fromCoreFrame(*frame);
    ASSERT(webFrame);


    //TODO 
    printf("a");
    websocket->document();
    printf("b\n");


    SecurityOrigin* origin = websocket->frame()->document()->securityOrigin();

    m_page->send(Messages::WebPageProxy::RequestWebSocketPermissionForFrame(websocketID, webFrame->frameID(), origin->databaseIdentifier()));
}

void WebSocketPermissionRequestManager::cancelRequestForWebSocket(WebSocket* websocket)
{
    uint64_t websocketID = m_websocketToIDMap.take(websocket);
    m_idToWebSocketMap.remove(websocketID);
}

void WebSocketPermissionRequestManager::didReceiveWebSocketPermissionDecision(uint64_t websocketID, bool allowed)
{
    printf("%d %d\n", websocketID, allowed);
    if(!websocketID){
        printf("websocketID");
        return;
    }
    WebSocket* websocket = m_idToWebSocketMap.take(websocketID);
    printf("did websocket : %p\n", websocket);    

    Page* page = websocket->page();
    if (!page)
        return;

    
    if (!websocket)
        return;
    m_websocketToIDMap.remove(websocket);

    if (!websocket)
        return;
    if(!websocket->document()){
        printf("websocket->document()\n");
        return;
    }
    if(!websocket->document()->page()){

        printf("websocket->document()->page()\n");
        return;
    }
    if(!WebSocketController::from(websocket->document()->page())){
        printf("WebSocketController::from(websocket->document()->page())\n");
        return;
    }

    //WebSocketController::from(page)->receivePermissionDecision(websocket, allowed);
    WebSocketController::from(websocket->document()->page())->receivePermissionDecision(websocket, allowed);
    printf("complete! \n");
}

} // namespace WebKit

#endif // ENABLE(WEB_SOCKETS)
