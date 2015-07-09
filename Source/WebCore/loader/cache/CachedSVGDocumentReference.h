/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CachedSVGDocumentReference_h
#define CachedSVGDocumentReference_h

#include "CachedResourceHandle.h"
#include "CachedSVGDocumentClient.h"
#include <wtf/text/WTFString.h>

namespace WebCore {

class CachedSVGDocument;
class CachedResourceLoader;

class CachedSVGDocumentReference : public CachedSVGDocumentClient {
public:
    CachedSVGDocumentReference(const String& url, CachedSVGDocumentClient* additionalDocumentClient = nullptr, bool canReuseResource = true);

    virtual ~CachedSVGDocumentReference();

    void load(CachedResourceLoader&);
    bool loadRequested() const { return m_loadRequested; }
    void setAcceptsAnyImageType() { m_acceptsAnyImageType = true; }

    CachedSVGDocument* document() { return m_document.get(); }

private:
    String m_url;
    CachedResourceHandle<CachedSVGDocument> m_document;
    bool m_loadRequested;
    CachedSVGDocumentClient* m_additionalDocumentClient;
    bool m_canReuseResource;
    bool m_acceptsAnyImageType { false };
};

};

#endif // CachedSVGDocumentReference_h
