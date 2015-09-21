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

#ifndef WebSocketPermissionRequestProxy_h
#define WebSocketPermissionRequestProxy_h

#include "APIObject.h"
#include <wtf/PassRefPtr.h>

namespace WebKit {

class WebSocketPermissionRequestManagerProxy;

class WebSocketPermissionRequestProxy : public API::ObjectImpl<API::Object::Type::WebSocketPermissionRequest> {
public:
    static Ref<WebSocketPermissionRequestProxy> create(WebSocketPermissionRequestManagerProxy* manager, uint64_t websocketID)
    {
        return adoptRef(*new WebSocketPermissionRequestProxy(manager, websocketID));
    }

    void allow();
    void deny();
    
    void invalidate();

private:
    WebSocketPermissionRequestProxy(WebSocketPermissionRequestManagerProxy*, uint64_t websocketID);

    WebSocketPermissionRequestManagerProxy* m_manager;
    uint64_t m_websocketID;
};

} // namespace WebKit

#endif // WebSocketPermissionRequestProxy_h
