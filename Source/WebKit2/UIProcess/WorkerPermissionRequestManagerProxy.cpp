/*
 * Copyright (C) 2015 Company 100, Inc. All rights reserved.
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

#include "config.h"
#include "WorkerPermissionRequestManagerProxy.h"

#include "WebPageMessages.h"
#include "WebPageProxy.h"
#include "WebProcessProxy.h"

namespace WebKit {

WorkerPermissionRequestManagerProxy::WorkerPermissionRequestManagerProxy(WebPageProxy& page)
    : m_page(page)
{
}

void WorkerPermissionRequestManagerProxy::invalidateRequests()
{
    for (auto& request : m_pendingRequests.values())
        request->invalidate();

    m_pendingRequests.clear();
}

PassRefPtr<WorkerPermissionRequestProxy> WorkerPermissionRequestManagerProxy::createRequest(uint64_t workerID)
{
    RefPtr<WorkerPermissionRequestProxy> request = WorkerPermissionRequestProxy::create(this, workerID);
    m_pendingRequests.add(workerID, request.get());
    return request.release();
}

void WorkerPermissionRequestManagerProxy::didReceiveWorkerPermissionDecision(uint64_t workerID, bool allowed)
{
    if (!m_page.isValid())
        return;

    auto it = m_pendingRequests.find(workerID);
    if (it == m_pendingRequests.end())
        return;

    m_page.process().send(Messages::WebPage::DidReceiveWorkerPermissionDecision(workerID, allowed), m_page.pageID());

    m_pendingRequests.remove(it);
}

} // namespace WebKit
