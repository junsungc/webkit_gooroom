/*
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
#include "WebNotificationManagerProxy.h"

#include "ImmutableArray.h"
#include "WebContext.h"
#include "WebNotification.h"
#include "WebNotificationManagerMessages.h"
#include "WebSecurityOrigin.h"
#include <WebCore/NotificationContents.h>

using namespace WTF;
using namespace WebCore;

namespace WebKit {

PassRefPtr<WebNotificationManagerProxy> WebNotificationManagerProxy::create(WebContext* context)
{
    return adoptRef(new WebNotificationManagerProxy(context));
}

WebNotificationManagerProxy::WebNotificationManagerProxy(WebContext* context)
    : m_context(context)
{
}

void WebNotificationManagerProxy::invalidate()
{
    m_provider.removeNotificationManager(this);
}

void WebNotificationManagerProxy::initializeProvider(const WKNotificationProvider *provider)
{
    m_provider.initialize(provider);
}

void WebNotificationManagerProxy::didReceiveMessage(CoreIPC::Connection* connection, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    didReceiveWebNotificationManagerProxyMessage(connection, messageID, arguments);
}

void WebNotificationManagerProxy::didReceiveSyncMessage(CoreIPC::Connection* connection, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments, OwnPtr<CoreIPC::ArgumentEncoder>& reply)
{
    didReceiveSyncWebNotificationManagerProxyMessage(connection, messageID, arguments, reply);
}

void WebNotificationManagerProxy::show(const String& title, const String& body, const String& originIdentifier, uint64_t notificationID)
{
    if (!isNotificationIDValid(notificationID))
        return;
    
    m_provider.addNotificationManager(this);

    RefPtr<WebNotification> notification = WebNotification::create(title, body, originIdentifier, notificationID);
    m_notifications.set(notificationID, notification);
    m_provider.show(notification.get());
}

void WebNotificationManagerProxy::cancel(uint64_t notificationID)
{
    if (!isNotificationIDValid(notificationID))
        return;

    RefPtr<WebNotification> notification = m_notifications.get(notificationID);
    if (!notification)
        return;

    m_provider.addNotificationManager(this);
    m_provider.cancel(notification.get());
}
    
void WebNotificationManagerProxy::didDestroyNotification(uint64_t notificationID)
{
    if (!isNotificationIDValid(notificationID))
        return;

    RefPtr<WebNotification> notification = m_notifications.take(notificationID);
    if (!notification)
        return;

    m_provider.didDestroyNotification(notification.get());
}

void WebNotificationManagerProxy::notificationPermissionLevel(const String& originIdentifier, uint64_t& permissionLevel)
{
    RefPtr<WebSecurityOrigin> origin = WebSecurityOrigin::create(originIdentifier);
    permissionLevel = m_provider.policyForNotificationPermissionAtOrigin(origin.get());
}

void WebNotificationManagerProxy::providerDidShowNotification(uint64_t notificationID)
{
    if (!m_context)
        return;
    
    m_context->sendToAllProcesses(Messages::WebNotificationManager::DidShowNotification(notificationID));
}

void WebNotificationManagerProxy::providerDidClickNotification(uint64_t notificationID)
{
    if (!m_context)
        return;
    
    m_context->sendToAllProcesses(Messages::WebNotificationManager::DidClickNotification(notificationID));
}


void WebNotificationManagerProxy::providerDidCloseNotifications(ImmutableArray* notificationIDs)
{
    if (!m_context)
        return;
    
    size_t size = notificationIDs->size();
    
    Vector<uint64_t> vectorNotificationIDs;
    vectorNotificationIDs.reserveInitialCapacity(size);
    
    for (size_t i = 0; i < size; ++i) {
        uint64_t notificationID = notificationIDs->at<WebUInt64>(i)->value();
        vectorNotificationIDs.append(notificationID);
    }
    
    if (vectorNotificationIDs.size())
        m_context->sendToAllProcesses(Messages::WebNotificationManager::DidCloseNotifications(vectorNotificationIDs));
}

} // namespace WebKit
