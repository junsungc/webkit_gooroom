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
#include "WorkerController.h"

#include "WorkerClient.h"

namespace WebCore {

WorkerController::WorkerController(Page& page, WorkerClient& client)
    : m_page(page)
    , m_client(client)
{
    m_page.addViewStateChangeObserver(*this);
}

WorkerController::~WorkerController()
{
}

void WorkerController::requestPermission(Worker* worker)
{
    if (!m_page.isVisible()) {
        m_pendedPermissionRequest.add(worker);
        return;
    }

    m_client.requestPermission(worker);
}

void WorkerController::cancelPermissionRequest(Worker* worker)
{
    if (m_pendedPermissionRequest.remove(worker))
        return;

    m_client.cancelPermissionRequest(worker);
}

void WorkerController::viewStateDidChange(ViewState::Flags, ViewState::Flags)
{
    if (!m_page.isVisible())
        return;

    HashSet<RefPtr<Worker>> pendedPermissionRequests = WTF::move(m_pendedPermissionRequest);
    for (auto& permissionRequest : pendedPermissionRequests)
        m_client.requestPermission(permissionRequest.get());
}

const char* WorkerController::supplementName()
{
    return "WorkerController";
}

void provideWorkerTo(Page* page, WorkerClient* client)
{
    ASSERT(page);
    ASSERT(client);
    Supplement<Page>::provideTo(page, WorkerController::supplementName(), std::make_unique<WorkerController>(*page, *client));
}

} // namespace WebCore
