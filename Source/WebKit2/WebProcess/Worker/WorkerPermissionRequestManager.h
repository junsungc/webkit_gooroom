/*
 * Copyright (C) 2015 Company100, Inc. All rights reserved.
 * Copyright (C) 2011 Apple Inc. All rights reserved.
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

#ifndef WorkerPermissionRequestManager_h
#define WorkerPermissionRequestManager_h

#include <wtf/HashMap.h>
#include <wtf/RefPtr.h>

namespace WebCore {
class Worker;
}

namespace WebKit {

class WebPage;

class WorkerPermissionRequestManager {
public:
    explicit WorkerPermissionRequestManager(WebPage*);

    void startRequestForWorker(WebCore::Worker*);
    void cancelRequestForWorker(WebCore::Worker*);

    void didReceiveWorkerPermissionDecision(uint64_t geolocationID, bool allowed);

private:
    typedef HashMap<uint64_t, WebCore::Worker*> IDToWorkerMap;
    typedef HashMap<WebCore::Worker*, uint64_t> WorkerToIDMap;
    IDToWorkerMap m_idToWorkerMap;
    WorkerToIDMap m_workerToIDMap;

    WebPage* m_page;
};

} // namespace WebKit

#endif // WorkerPermissionRequestManager_h
