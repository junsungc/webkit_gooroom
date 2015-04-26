/*
    Copyright (C) 2010 Rob Buis <rwlbuis@gmail.com>
    Copyright (C) 2011 Cosmin Truta <ctruta@gmail.com>
    Copyright (C) 2012 University of Szeged
    Copyright (C) 2012 Renata Hodovan <reni@webkit.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef CachedSVGDocument_h
#define CachedSVGDocument_h

#include "CachedResource.h"
#include "CachedResourceHandle.h"
#include "SVGDocument.h"
#include "TextResourceDecoder.h"

namespace WebCore {

class CachedSVGDocument final : public CachedResource {
public:
    explicit CachedSVGDocument(const ResourceRequest&, SessionID);
    virtual ~CachedSVGDocument();

    SVGDocument* document() const { return m_document.get(); }
    void setShouldCreateFrameForDocument(bool shouldCreate) { m_shouldCreateFrameForDocument = shouldCreate; }
    virtual bool canReuse(const ResourceRequest&) const override { return m_canReuseResource; }
    void setCanReuse(bool canReuseResource) { m_canReuseResource = canReuseResource; };

private:
    virtual bool mayTryReplaceEncodedData() const override { return true; }
    virtual void setEncoding(const String&) override;
    virtual String encoding() const override;
    virtual void finishLoading(SharedBuffer*) override;

    RefPtr<SVGDocument> m_document;
    RefPtr<TextResourceDecoder> m_decoder;
    std::unique_ptr<Page> m_page;
    bool m_shouldCreateFrameForDocument;
    bool m_canReuseResource;
};

} // namespace WebCore

SPECIALIZE_TYPE_TRAITS_CACHED_RESOURCE(CachedSVGDocument, CachedResource::SVGDocumentResource)

#endif // CachedSVGDocument_h
