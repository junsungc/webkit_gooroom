/*
 * Copyright (C) 2014 Apple Inc. All rights reserved.
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

#ifndef NetworkCache_h
#define NetworkCache_h

#if ENABLE(NETWORK_CACHE)

#include "NetworkCacheStorage.h"
#include "ShareableResource.h"
#include <WebCore/ResourceResponse.h>
#include <wtf/text/WTFString.h>

namespace WebCore {
class ResourceRequest;
class SharedBuffer;
class URL;
}

namespace WebKit {

class NetworkCacheStatistics;

class NetworkCache {
    WTF_MAKE_NONCOPYABLE(NetworkCache);
    friend class WTF::NeverDestroyed<NetworkCache>;
public:
    static NetworkCache& singleton();

    bool initialize(const String& cachePath, bool enableEfficacyLogging);
    void setMaximumSize(size_t);

    bool isEnabled() const { return !!m_storage; }

    struct Entry {
        NetworkCacheStorage::Entry storageEntry;
        WebCore::ResourceResponse response;
        RefPtr<WebCore::SharedBuffer> buffer;
#if ENABLE(SHAREABLE_RESOURCE)
        ShareableResource::Handle shareableResourceHandle;
#endif
        bool needsRevalidation;
    };
    // Completion handler may get called back synchronously on failure.
    void retrieve(const WebCore::ResourceRequest&, uint64_t webPageID, std::function<void (std::unique_ptr<Entry>)>);

    struct MappedBody {
#if ENABLE(SHAREABLE_RESOURCE)
        RefPtr<ShareableResource> shareableResource;
        ShareableResource::Handle shareableResourceHandle;
#endif
    };
    void store(const WebCore::ResourceRequest&, const WebCore::ResourceResponse&, RefPtr<WebCore::SharedBuffer>&&, std::function<void (MappedBody&)>);
    void update(const WebCore::ResourceRequest&, const Entry&, const WebCore::ResourceResponse& validatingResponse);

    void clear();

    String storagePath() const;

private:
    NetworkCache() = default;
    ~NetworkCache() = delete;

    std::unique_ptr<NetworkCacheStorage> m_storage;
    std::unique_ptr<NetworkCacheStatistics> m_statistics;
};

}
#endif
#endif
