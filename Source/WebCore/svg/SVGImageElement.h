/*
 * Copyright (C) 2004, 2005, 2006, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SVGImageElement_h
#define SVGImageElement_h

#if ENABLE(SVG)
#include "SVGAnimatedBoolean.h"
#include "SVGAnimatedLength.h"
#include "SVGAnimatedPreserveAspectRatio.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGGraphicsElement.h"
#include "SVGImageLoader.h"
#include "SVGURIReference.h"

namespace WebCore {

class SVGImageElement FINAL : public SVGGraphicsElement,
                              public SVGExternalResourcesRequired,
                              public SVGURIReference {
public:
    static PassRefPtr<SVGImageElement> create(const QualifiedName&, Document&);

private:
    SVGImageElement(const QualifiedName&, Document&);
    
    virtual bool isValid() const { return SVGTests::isValid(); }
    virtual bool supportsFocus() const OVERRIDE { return true; }

    bool isSupportedAttribute(const QualifiedName&);
    virtual void parseAttribute(const QualifiedName&, const AtomicString&) OVERRIDE;
    virtual bool isPresentationAttribute(const QualifiedName&) const OVERRIDE;
    virtual void collectStyleForPresentationAttribute(const QualifiedName&, const AtomicString&, MutableStyleProperties&) OVERRIDE;
    virtual void svgAttributeChanged(const QualifiedName&);

    virtual void didAttachRenderers() OVERRIDE;
    virtual InsertionNotificationRequest insertedInto(ContainerNode&) OVERRIDE;

    virtual RenderPtr<RenderElement> createElementRenderer(PassRef<RenderStyle>) OVERRIDE;

    virtual const AtomicString& imageSourceURL() const OVERRIDE;
    virtual void addSubresourceAttributeURLs(ListHashSet<URL>&) const;

    virtual bool haveLoadedRequiredResources();

    virtual bool selfHasRelativeLengths() const;
    virtual void didMoveToNewDocument(Document* oldDocument) OVERRIDE;

    BEGIN_DECLARE_ANIMATED_PROPERTIES(SVGImageElement)
        DECLARE_ANIMATED_LENGTH(X, x)
        DECLARE_ANIMATED_LENGTH(Y, y)
        DECLARE_ANIMATED_LENGTH(Width, width)
        DECLARE_ANIMATED_LENGTH(Height, height)
        DECLARE_ANIMATED_PRESERVEASPECTRATIO(PreserveAspectRatio, preserveAspectRatio)
        DECLARE_ANIMATED_STRING(Href, href)
        DECLARE_ANIMATED_BOOLEAN(ExternalResourcesRequired, externalResourcesRequired)
    END_DECLARE_ANIMATED_PROPERTIES

    SVGImageLoader m_imageLoader;
};

NODE_TYPE_CASTS(SVGImageElement)

} // namespace WebCore

#endif // ENABLE(SVG)
#endif
