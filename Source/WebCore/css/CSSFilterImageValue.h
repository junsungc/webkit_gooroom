/*
 * Copyright (C) 2013 Adobe Systems Incorporated. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef CSSFilterImageValue_h
#define CSSFilterImageValue_h

#if ENABLE(CSS_FILTERS)

#include "CSSImageGeneratorValue.h"
#include "CSSPrimitiveValue.h"
#include "CachedImageClient.h"
#include "CachedResourceHandle.h"
#include "FilterOperations.h"
#include "Image.h"
#include "ImageObserver.h"

namespace WebCore {

class CachedImage;
class FilterSubimageObserverProxy;
class RenderObject;
class Document;
class StyleResolver;

class CSSFilterImageValue : public CSSImageGeneratorValue {
    friend class FilterSubimageObserverProxy;
public:
    static PassRefPtr<CSSFilterImageValue> create(PassRefPtr<CSSValue> imageValue, PassRefPtr<CSSValue> filterValue)
    {
        return adoptRef(new CSSFilterImageValue(imageValue, filterValue));
    }

    ~CSSFilterImageValue();

    String customCssText() const;

    PassRefPtr<Image> image(RenderObject*, const IntSize&);
    bool isFixedSize() const { return true; }
    IntSize fixedSize(const RenderObject*);

    bool isPending() const;
    bool knownToBeOpaque(const RenderObject*) const;

    void loadSubimages(CachedResourceLoader*);

    bool hasFailedOrCanceledSubresources() const;

    bool equals(const CSSFilterImageValue&) const;

    void createFilterOperations(StyleResolver*);

private:
    CSSFilterImageValue(PassRefPtr<CSSValue> imageValue, PassRefPtr<CSSValue> filterValue)
        : CSSImageGeneratorValue(FilterImageClass)
        , m_imageValue(imageValue)
        , m_filterValue(filterValue)
        , m_filterSubimageObserver(this)
    {
    }

    class FilterSubimageObserverProxy : public CachedImageClient {
    public:
        FilterSubimageObserverProxy(CSSFilterImageValue* ownerValue)
            : m_ownerValue(ownerValue)
            , m_ready(false)
        {
        }

        virtual ~FilterSubimageObserverProxy() { }
        virtual void imageChanged(CachedImage*, const IntRect* = 0) OVERRIDE;
        void setReady(bool ready) { m_ready = ready; }
    private:
        CSSFilterImageValue* m_ownerValue;
        bool m_ready;
    };

    void filterImageChanged(const IntRect&);

    RefPtr<CSSValue> m_imageValue;
    RefPtr<CSSValue> m_filterValue;

    FilterOperations m_filterOperations;

    CachedResourceHandle<CachedImage> m_cachedImage;
    RefPtr<Image> m_generatedImage;

    FilterSubimageObserverProxy m_filterSubimageObserver;
};

} // namespace WebCore

#endif // ENABLE(CSS_FILTERS)

#endif // CSSFilterImageValue_h
