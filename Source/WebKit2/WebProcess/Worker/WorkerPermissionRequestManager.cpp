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

#include "config.h"
#include "WorkerPermissionRequestManager.h"

#include "WebCoreArgumentCoders.h"
#include "WebFrame.h"
#include "WebPage.h"
#include "WebPageProxyMessages.h"
#include <WebCore/Document.h>
#include <WebCore/Frame.h>
#include <WebCore/FrameLoader.h>
#include <WebCore/SecurityOrigin.h>
#include <WebCore/Worker.h>

using namespace WebCore;

namespace WebKit {

static uint64_t generateWorkerID()
{
    static uint64_t uniqueWorkerID = 1;
    return uniqueWorkerID++;
}

WorkerPermissionRequestManager::WorkerPermissionRequestManager(WebPage* page)
    : m_page(page)
{
}

void WorkerPermissionRequestManager::startRequestForWorker(Worker* worker)
{
    Frame* frame = worker->frame();
    if (!frame) {
        worker->notifyPermissionDecision(false);
        return;
    }

    uint64_t workerID = generateWorkerID();

    m_workerToIDMap.set(worker, workerID);
    m_idToWorkerMap.set(workerID, worker);

    WebFrame* webFrame = WebFrame::fromCoreFrame(*worker->frame());
    ASSERT(webFrame);

    SecurityOrigin* origin = worker->frame()->document()->securityOrigin();

    m_page->send(Messages::WebPageProxy::RequestWorkerPermissionForFrame(workerID, webFrame->frameID(), origin->databaseIdentifier()));
}

void WorkerPermissionRequestManager::cancelRequestForWorker(Worker* worker)
{
    uint64_t workerID = m_workerToIDMap.take(worker);
    if (!workerID)
        return;
    m_idToWorkerMap.remove(workerID);
}

void WorkerPermissionRequestManager::didReceiveWorkerPermissionDecision(uint64_t workerID, bool allowed)
{
    Worker* worker = m_idToWorkerMap.take(workerID);
    if (!worker)
        return;
    m_workerToIDMap.remove(worker);

    worker->notifyPermissionDecision(allowed);
}

} // namespace WebKit
