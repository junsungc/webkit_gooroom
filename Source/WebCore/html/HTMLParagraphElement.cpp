/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2010 Apple Inc. All rights reserved.
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
 *
 */

#include "config.h"
#include "HTMLParagraphElement.h"

#include "Attribute.h"
#include "CSSPropertyNames.h"
#include "CSSValueKeywords.h"
#include "Document.h"
#include "HTMLNames.h"

namespace WebCore {

using namespace HTMLNames;

inline HTMLParagraphElement::HTMLParagraphElement(const QualifiedName& tagName, Document* document)
    : HTMLElement(tagName, document)
{
    ASSERT(hasTagName(pTag));
}

PassRefPtr<HTMLParagraphElement> HTMLParagraphElement::create(const QualifiedName& tagName, Document* document)
{
    return adoptRef(new HTMLParagraphElement(tagName, document));
}

void HTMLParagraphElement::parseAttribute(Attribute* attr)
{
    if (attr->name() == alignAttr) {
        if (attr->value().isNull())
            removeCSSProperty(CSSPropertyTextAlign);
        else if (equalIgnoringCase(attr->value(), "middle") || equalIgnoringCase(attr->value(), "center"))
            addCSSProperty(CSSPropertyTextAlign, CSSValueWebkitCenter);
        else if (equalIgnoringCase(attr->value(), "left"))
            addCSSProperty(CSSPropertyTextAlign, CSSValueWebkitLeft);
        else if (equalIgnoringCase(attr->value(), "right"))
            addCSSProperty(CSSPropertyTextAlign, CSSValueWebkitRight);
        else
            addCSSProperty(CSSPropertyTextAlign, attr->value());
    } else
        HTMLElement::parseAttribute(attr);
}

}
