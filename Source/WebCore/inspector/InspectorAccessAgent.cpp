/*
 * Copyright (C) 2015 Company 100, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "InspectorAccessAgent.h"

#include "InstrumentingAgents.h"

using namespace Inspector;

namespace WebCore {

InspectorAccessAgent::InspectorAccessAgent(WebAgentContext& context)
    : InspectorAgentBase(ASCIILiteral("ACCESS"), context)
    , m_frontendDispatcher(std::make_unique<AccessFrontendDispatcher>(context.frontendRouter))
{
    m_instrumentingAgents.setInspectorAccessAgent(this);
}

InspectorAccessAgent::~InspectorAccessAgent()
{
    m_instrumentingAgents.setInspectorAccessAgent(nullptr);
}

void InspectorAccessAgent::didCreateFrontendAndBackend(Inspector::FrontendRouter*, Inspector::BackendDispatcher*)
{
}

void InspectorAccessAgent::willDestroyFrontendAndBackend(Inspector::DisconnectReason)
{
}

void InspectorAccessAgent::didSendWorkerPermissionRequest(int workerId, const String& url)
{
    m_frontendDispatcher->didSendWorkerPermissionRequest(workerId, url);
}

void InspectorAccessAgent::didReceiveWorkerPermissionResponse(int workerId, bool allowed)
{
    m_frontendDispatcher->didReceiveWorkerPermissionResponse(workerId, allowed);
}

void InspectorAccessAgent::didSendWebSocketPermissionRequest(int websocketId, const String& url)
{
    m_frontendDispatcher->didSendWebSocketPermissionRequest(websocketId, url);
}

void InspectorAccessAgent::didReceiveWebSocketPermissionResponse(int websocketId, bool allowed)
{
    m_frontendDispatcher->didReceiveWebSocketPermissionResponse(websocketId, allowed);
}

void InspectorAccessAgent::didWebSocketClose(int websocketId)
{
    m_frontendDispatcher->didWebSocketClose(websocketId);
}

} // namespace WebCore
