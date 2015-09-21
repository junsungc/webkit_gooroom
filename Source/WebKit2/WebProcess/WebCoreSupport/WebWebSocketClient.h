#ifndef WebWebSocketClient_h
#define WebWebSocketClient_h

#include <WebCore/WebSocketClient.h>

namespace WebKit {

class WebPage;

class WebWebSocketClient : public WebCore::WebSocketClient {
public:
    WebWebSocketClient(WebPage* page)
        : m_page(page)
    {
    }

    virtual ~WebWebSocketClient();

private:
    virtual void websocketDestroyed() override;

    virtual void requestPermission(WebCore::WebSocket*) override;
    virtual void cancelPermissionRequest(WebCore::WebSocket*) override;


    WebPage* m_page;
};

} // namespace WebKit

#endif
