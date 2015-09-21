#include "config.h"
#include "WebWebSocketClient.h"

#if ENABLE(WEB_SOCKETS)
#include "WebPage.h"
#include "WebProcess.h"
#include "WebWebSocketManager.h"
#include <WebCore/WebSocket.h>

using namespace WebCore;

namespace WebKit {

WebWebSocketClient::~WebWebSocketClient()
{
}

void WebWebSocketClient::websocketDestroyed()
{
    WebProcess::singleton().supplement<WebWebSocketManager>()->unregisterWebPage(m_page);
    delete this;
}

void WebWebSocketClient::requestPermission(WebSocket* websocket)
{
    m_page->websocketPermissionRequestManager().startRequestForWebSocket(websocket);
}

void WebWebSocketClient::cancelPermissionRequest(WebSocket* websocket)
{
    m_page->websocketPermissionRequestManager().cancelRequestForWebSocket(websocket);
}

} // namespace WebKit

#endif // ENABLE(GEOLOCATION)
