/*
 * Copyright (C) 2012 Adobe Systems Incorporated. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER “AS IS” AND ANY
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

#include "config.h"
#include "BasicShapeFunctions.h"

#include "BasicShapes.h"
#include "CSSBasicShapes.h"
#include "CSSPrimitiveValueMappings.h"
#include "CSSValuePool.h"
#include "Pair.h"
#include "RenderStyle.h"

namespace WebCore {

PassRefPtr<CSSPrimitiveValue> valueForBox(LayoutBox box)
{
    switch (box) {
    case ContentBox:
        return cssValuePool().createIdentifierValue(CSSValueContentBox);
    case PaddingBox:
        return cssValuePool().createIdentifierValue(CSSValuePaddingBox);
    case BorderBox:
        return cssValuePool().createIdentifierValue(CSSValueBorderBox);
    case MarginBox:
        return cssValuePool().createIdentifierValue(CSSValueMarginBox);
    case BoundingBox:
        return cssValuePool().createIdentifierValue(CSSValueBoundingBox);
    case BoxMissing:
        return nullptr;
    }
    ASSERT_NOT_REACHED();
    return nullptr;
}

LayoutBox boxForValue(const CSSPrimitiveValue* value)
{
    if (!value)
        return BoxMissing;

    switch (value->getValueID()) {
    case CSSValueContentBox:
        return ContentBox;
    case CSSValuePaddingBox:
        return PaddingBox;
    case CSSValueBorderBox:
        return BorderBox;
    case CSSValueMarginBox:
        return MarginBox;
    case CSSValueBoundingBox:
        return BoundingBox;
    default:
        ASSERT_NOT_REACHED();
        return BoxMissing;
    }
}

static PassRefPtr<CSSPrimitiveValue> valueForCenterCoordinate(CSSValuePool& pool, const RenderStyle* style, const BasicShapeCenterCoordinate& center)
{
    CSSValueID keyword = CSSValueInvalid;
    switch (center.keyword()) {
    case BasicShapeCenterCoordinate::None:
        return pool.createValue(center.length(), style);
    case BasicShapeCenterCoordinate::Top:
        keyword = CSSValueTop;
        break;
    case BasicShapeCenterCoordinate::Right:
        keyword = CSSValueRight;
        break;
    case BasicShapeCenterCoordinate::Bottom:
        keyword = CSSValueBottom;
        break;
    case BasicShapeCenterCoordinate::Left:
        keyword = CSSValueLeft;
        break;
    }

    return pool.createValue(Pair::create(pool.createIdentifierValue(keyword), pool.createValue(center.length(), style)));
}

static PassRefPtr<CSSPrimitiveValue> basicShapeRadiusToCSSValue(const RenderStyle* style, CSSValuePool& pool, const BasicShapeRadius& radius)
{
    switch (radius.type()) {
    case BasicShapeRadius::Value:
        return pool.createValue(radius.value(), style);
    case BasicShapeRadius::ClosestSide:
        return pool.createIdentifierValue(CSSValueClosestSide);
    case BasicShapeRadius::FarthestSide:
        return pool.createIdentifierValue(CSSValueFarthestSide);
    }

    ASSERT_NOT_REACHED();
    return 0;
}

PassRefPtr<CSSValue> valueForBasicShape(const RenderStyle* style, const BasicShape* basicShape)
{
    CSSValuePool& pool = cssValuePool();

    RefPtr<CSSBasicShape> basicShapeValue;
    switch (basicShape->type()) {
    case BasicShape::BasicShapeRectangleType: {
        const BasicShapeRectangle* rectangle = static_cast<const BasicShapeRectangle*>(basicShape);
        RefPtr<CSSBasicShapeRectangle> rectangleValue = CSSBasicShapeRectangle::create();

        rectangleValue->setX(pool.createValue(rectangle->x(), style));
        rectangleValue->setY(pool.createValue(rectangle->y(), style));
        rectangleValue->setWidth(pool.createValue(rectangle->width(), style));
        rectangleValue->setHeight(pool.createValue(rectangle->height(), style));
        rectangleValue->setRadiusX(pool.createValue(rectangle->cornerRadiusX(), style));
        rectangleValue->setRadiusY(pool.createValue(rectangle->cornerRadiusY(), style));

        basicShapeValue = rectangleValue.release();
        break;
    }
    case BasicShape::DeprecatedBasicShapeCircleType: {
        const DeprecatedBasicShapeCircle* circle = static_cast<const DeprecatedBasicShapeCircle*>(basicShape);
        RefPtr<CSSDeprecatedBasicShapeCircle> circleValue = CSSDeprecatedBasicShapeCircle::create();

        circleValue->setCenterX(pool.createValue(circle->centerX(), style));
        circleValue->setCenterY(pool.createValue(circle->centerY(), style));
        circleValue->setRadius(pool.createValue(circle->radius(), style));

        basicShapeValue = circleValue.release();
        break;
    }
    case BasicShape::BasicShapeCircleType: {
        const BasicShapeCircle* circle = static_cast<const BasicShapeCircle*>(basicShape);
        RefPtr<CSSBasicShapeCircle> circleValue = CSSBasicShapeCircle::create();

        circleValue->setCenterX(valueForCenterCoordinate(pool, style, circle->centerX()));
        circleValue->setCenterY(valueForCenterCoordinate(pool, style, circle->centerY()));
        circleValue->setRadius(basicShapeRadiusToCSSValue(style, pool, circle->radius()));
        basicShapeValue = circleValue.release();
        break;
    }
    case BasicShape::DeprecatedBasicShapeEllipseType: {
        const DeprecatedBasicShapeEllipse* ellipse = static_cast<const DeprecatedBasicShapeEllipse*>(basicShape);
        RefPtr<CSSDeprecatedBasicShapeEllipse> ellipseValue = CSSDeprecatedBasicShapeEllipse::create();

        ellipseValue->setCenterX(pool.createValue(ellipse->centerX(), style));
        ellipseValue->setCenterY(pool.createValue(ellipse->centerY(), style));
        ellipseValue->setRadiusX(pool.createValue(ellipse->radiusX(), style));
        ellipseValue->setRadiusY(pool.createValue(ellipse->radiusY(), style));

        basicShapeValue = ellipseValue.release();
        break;
    }
    case BasicShape::BasicShapeEllipseType: {
        const BasicShapeEllipse* ellipse = static_cast<const BasicShapeEllipse*>(basicShape);
        RefPtr<CSSBasicShapeEllipse> ellipseValue = CSSBasicShapeEllipse::create();

        ellipseValue->setCenterX(valueForCenterCoordinate(pool, style, ellipse->centerX()));
        ellipseValue->setCenterY(valueForCenterCoordinate(pool, style, ellipse->centerY()));
        ellipseValue->setRadiusX(basicShapeRadiusToCSSValue(style, pool, ellipse->radiusX()));
        ellipseValue->setRadiusY(basicShapeRadiusToCSSValue(style, pool, ellipse->radiusY()));
        basicShapeValue = ellipseValue.release();
        break;
    }
    case BasicShape::BasicShapePolygonType: {
        const BasicShapePolygon* polygon = static_cast<const BasicShapePolygon*>(basicShape);
        RefPtr<CSSBasicShapePolygon> polygonValue = CSSBasicShapePolygon::create();

        polygonValue->setWindRule(polygon->windRule());
        const Vector<Length>& values = polygon->values();
        for (unsigned i = 0; i < values.size(); i += 2)
            polygonValue->appendPoint(pool.createValue(values.at(i), style), pool.createValue(values.at(i + 1), style));

        basicShapeValue = polygonValue.release();
        break;
    }
    case BasicShape::BasicShapeInsetRectangleType: {
        const BasicShapeInsetRectangle* rectangle = static_cast<const BasicShapeInsetRectangle*>(basicShape);
        RefPtr<CSSBasicShapeInsetRectangle> rectangleValue = CSSBasicShapeInsetRectangle::create();

        rectangleValue->setTop(pool.createValue(rectangle->top(), style));
        rectangleValue->setRight(pool.createValue(rectangle->right(), style));
        rectangleValue->setBottom(pool.createValue(rectangle->bottom(), style));
        rectangleValue->setLeft(pool.createValue(rectangle->left(), style));
        rectangleValue->setRadiusX(pool.createValue(rectangle->cornerRadiusX(), style));
        rectangleValue->setRadiusY(pool.createValue(rectangle->cornerRadiusY(), style));

        basicShapeValue = rectangleValue.release();
        break;
    }
    case BasicShape::BasicShapeInsetType: {
        const BasicShapeInset* inset = static_cast<const BasicShapeInset*>(basicShape);
        RefPtr<CSSBasicShapeInset> insetValue = CSSBasicShapeInset::create();

        insetValue->setTop(pool.createValue(inset->top()));
        insetValue->setRight(pool.createValue(inset->right()));
        insetValue->setBottom(pool.createValue(inset->bottom()));
        insetValue->setLeft(pool.createValue(inset->left()));

        insetValue->setTopLeftRadius(pool.createValue(inset->topLeftRadius()));
        insetValue->setTopRightRadius(pool.createValue(inset->topRightRadius()));
        insetValue->setBottomRightRadius(pool.createValue(inset->bottomRightRadius()));
        insetValue->setBottomLeftRadius(pool.createValue(inset->bottomLeftRadius()));

        basicShapeValue = insetValue.release();
        break;
    }
    default:
        break;
    }

    basicShapeValue->setLayoutBox(valueForBox(basicShape->layoutBox()));

    return pool.createValue(basicShapeValue.release());
}

static Length convertToLength(const RenderStyle* style, const RenderStyle* rootStyle, CSSPrimitiveValue* value)
{
    return value->convertToLength<FixedIntegerConversion | FixedFloatConversion | PercentConversion | CalculatedConversion | ViewportPercentageConversion>(style, rootStyle, style->effectiveZoom());
}

static BasicShapeCenterCoordinate convertToCenterCoordinate(const RenderStyle* style, const RenderStyle* rootStyle, CSSPrimitiveValue* value)
{
    if (Pair* pair = value->getPairValue()) {
        BasicShapeCenterCoordinate::Keyword keyword = BasicShapeCenterCoordinate::None;
        switch (pair->first()->getValueID()) {
        case CSSValueTop:
            keyword = BasicShapeCenterCoordinate::Top;
            break;
        case CSSValueRight:
            keyword = BasicShapeCenterCoordinate::Right;
            break;
        case CSSValueBottom:
            keyword = BasicShapeCenterCoordinate::Bottom;
            break;
        case CSSValueLeft:
            keyword = BasicShapeCenterCoordinate::Left;
            break;
        default:
            ASSERT_NOT_REACHED();
            break;
        }
        return BasicShapeCenterCoordinate(keyword, convertToLength(style, rootStyle, pair->second()));
    }

    return BasicShapeCenterCoordinate(convertToLength(style, rootStyle, value));
}

static BasicShapeRadius cssValueToBasicShapeRadius(const RenderStyle* style, const RenderStyle* rootStyle, PassRefPtr<CSSPrimitiveValue> radius)
{
    if (!radius)
        return BasicShapeRadius(BasicShapeRadius::ClosestSide);

    if (radius->isValueID()) {
        switch (radius->getValueID()) {
        case CSSValueClosestSide:
            return BasicShapeRadius(BasicShapeRadius::ClosestSide);
        case CSSValueFarthestSide:
            return BasicShapeRadius(BasicShapeRadius::FarthestSide);
        default:
            ASSERT_NOT_REACHED();
            break;
        }
    }

    return BasicShapeRadius(convertToLength(style, rootStyle, radius.get()));
}

PassRefPtr<BasicShape> basicShapeForValue(const RenderStyle* style, const RenderStyle* rootStyle, const CSSBasicShape* basicShapeValue)
{
    RefPtr<BasicShape> basicShape;

    switch (basicShapeValue->type()) {
    case CSSBasicShape::CSSBasicShapeRectangleType: {
        const CSSBasicShapeRectangle* rectValue = static_cast<const CSSBasicShapeRectangle *>(basicShapeValue);
        RefPtr<BasicShapeRectangle> rect = BasicShapeRectangle::create();

        rect->setX(convertToLength(style, rootStyle, rectValue->x()));
        rect->setY(convertToLength(style, rootStyle, rectValue->y()));
        rect->setWidth(convertToLength(style, rootStyle, rectValue->width()));
        rect->setHeight(convertToLength(style, rootStyle, rectValue->height()));
        if (rectValue->radiusX()) {
            Length radiusX = convertToLength(style, rootStyle, rectValue->radiusX());
            rect->setCornerRadiusX(radiusX);
            if (rectValue->radiusY())
                rect->setCornerRadiusY(convertToLength(style, rootStyle, rectValue->radiusY()));
            else
                rect->setCornerRadiusY(radiusX);
        } else {
            rect->setCornerRadiusX(Length(0, Fixed));
            rect->setCornerRadiusY(Length(0, Fixed));
        }
        basicShape = rect.release();
        break;
    }
    case CSSBasicShape::CSSDeprecatedBasicShapeCircleType: {
        const CSSDeprecatedBasicShapeCircle* circleValue = static_cast<const CSSDeprecatedBasicShapeCircle *>(basicShapeValue);
        RefPtr<DeprecatedBasicShapeCircle> circle = DeprecatedBasicShapeCircle::create();

        circle->setCenterX(convertToLength(style, rootStyle, circleValue->centerX()));
        circle->setCenterY(convertToLength(style, rootStyle, circleValue->centerY()));
        circle->setRadius(convertToLength(style, rootStyle, circleValue->radius()));

        basicShape = circle.release();
        break;
    }
    case CSSBasicShape::CSSBasicShapeCircleType: {
        const CSSBasicShapeCircle* circleValue = static_cast<const CSSBasicShapeCircle *>(basicShapeValue);
        RefPtr<BasicShapeCircle> circle = BasicShapeCircle::create();

        if (circleValue->centerX() && circleValue->centerY()) {
            circle->setCenterX(convertToCenterCoordinate(style, rootStyle, circleValue->centerX()));
            circle->setCenterY(convertToCenterCoordinate(style, rootStyle, circleValue->centerY()));
        } else {
            circle->setCenterX(BasicShapeCenterCoordinate(Length(50, Percent)));
            circle->setCenterY(BasicShapeCenterCoordinate(Length(50, Percent)));
        }
        circle->setRadius(cssValueToBasicShapeRadius(style, rootStyle, circleValue->radius()));

        basicShape = circle.release();
        break;
    }
    case CSSBasicShape::CSSDeprecatedBasicShapeEllipseType: {
        const CSSDeprecatedBasicShapeEllipse* ellipseValue = static_cast<const CSSDeprecatedBasicShapeEllipse *>(basicShapeValue);
        RefPtr<DeprecatedBasicShapeEllipse> ellipse = DeprecatedBasicShapeEllipse::create();

        ellipse->setCenterX(convertToLength(style, rootStyle, ellipseValue->centerX()));
        ellipse->setCenterY(convertToLength(style, rootStyle, ellipseValue->centerY()));
        ellipse->setRadiusX(convertToLength(style, rootStyle, ellipseValue->radiusX()));
        ellipse->setRadiusY(convertToLength(style, rootStyle, ellipseValue->radiusY()));

        basicShape = ellipse.release();
        break;
    }
    case CSSBasicShape::CSSBasicShapeEllipseType: {
        const CSSBasicShapeEllipse* ellipseValue = static_cast<const CSSBasicShapeEllipse *>(basicShapeValue);
        RefPtr<BasicShapeEllipse> ellipse = BasicShapeEllipse::create();

        if (ellipseValue->centerX() && ellipseValue->centerY()) {
            ellipse->setCenterX(convertToCenterCoordinate(style, rootStyle, ellipseValue->centerX()));
            ellipse->setCenterY(convertToCenterCoordinate(style, rootStyle, ellipseValue->centerY()));
        } else {
            ellipse->setCenterX(BasicShapeCenterCoordinate(Length(50, Percent)));
            ellipse->setCenterY(BasicShapeCenterCoordinate(Length(50, Percent)));
        }
        ellipse->setRadiusX(cssValueToBasicShapeRadius(style, rootStyle, ellipseValue->radiusX()));
        ellipse->setRadiusY(cssValueToBasicShapeRadius(style, rootStyle, ellipseValue->radiusY()));

        basicShape = ellipse.release();
        break;
    }
    case CSSBasicShape::CSSBasicShapePolygonType: {
        const CSSBasicShapePolygon* polygonValue = static_cast<const CSSBasicShapePolygon *>(basicShapeValue);
        RefPtr<BasicShapePolygon> polygon = BasicShapePolygon::create();

        polygon->setWindRule(polygonValue->windRule());
        const Vector<RefPtr<CSSPrimitiveValue>>& values = polygonValue->values();
        for (unsigned i = 0; i < values.size(); i += 2)
            polygon->appendPoint(convertToLength(style, rootStyle, values.at(i).get()), convertToLength(style, rootStyle, values.at(i + 1).get()));

        basicShape = polygon.release();
        break;
    }
    case CSSBasicShape::CSSBasicShapeInsetRectangleType: {
        const CSSBasicShapeInsetRectangle* rectValue = static_cast<const CSSBasicShapeInsetRectangle *>(basicShapeValue);
        RefPtr<BasicShapeInsetRectangle> rect = BasicShapeInsetRectangle::create();

        rect->setTop(convertToLength(style, rootStyle, rectValue->top()));
        rect->setRight(convertToLength(style, rootStyle, rectValue->right()));
        rect->setBottom(convertToLength(style, rootStyle, rectValue->bottom()));
        rect->setLeft(convertToLength(style, rootStyle, rectValue->left()));
        if (rectValue->radiusX()) {
            Length radiusX = convertToLength(style, rootStyle, rectValue->radiusX());
            rect->setCornerRadiusX(radiusX);
            if (rectValue->radiusY())
                rect->setCornerRadiusY(convertToLength(style, rootStyle, rectValue->radiusY()));
            else
                rect->setCornerRadiusY(radiusX);
        } else {
            rect->setCornerRadiusX(Length(0, Fixed));
            rect->setCornerRadiusY(Length(0, Fixed));
        }
        basicShape = rect.release();
        break;
    }
    case CSSBasicShape::CSSBasicShapeInsetType: {
        const CSSBasicShapeInset* rectValue = static_cast<const CSSBasicShapeInset* >(basicShapeValue);
        RefPtr<BasicShapeInset> rect = BasicShapeInset::create();

        if (rectValue->top())
            rect->setTop(convertToLength(style, rootStyle, rectValue->top()));
        else {
            rect->setTop(Length(0, Fixed));
            return rect;
        }
        if (rectValue->right())
            rect->setRight(convertToLength(style, rootStyle, rectValue->right()));
        else
            rect->setRight(Length(0, Fixed));

        if (rectValue->bottom())
            rect->setBottom(convertToLength(style, rootStyle, rectValue->bottom()));
        else
            rect->setBottom(Length(0, Fixed));

        if (rectValue->left())
            rect->setLeft(convertToLength(style, rootStyle, rectValue->left()));
        else
            rect->setLeft(Length(0, Fixed));

        if (rectValue->topLeftRadius()) {
            Pair* topLeftRadius = rectValue->topLeftRadius()->getPairValue();
            rect->setTopLeftRadius(LengthSize(convertToLength(style, rootStyle, topLeftRadius->first()), convertToLength(style, rootStyle, topLeftRadius->second())));
        } else
            rect->setTopLeftRadius(LengthSize(Length(0, Fixed), Length(0, Fixed)));

        if (rectValue->topRightRadius()) {
            Pair* topRightRadius = rectValue->topRightRadius()->getPairValue();
            rect->setTopRightRadius(LengthSize(convertToLength(style, rootStyle, topRightRadius->first()), convertToLength(style, rootStyle, topRightRadius->second())));
        } else
            rect->setTopRightRadius(LengthSize(Length(0, Fixed), Length(0, Fixed)));

        if (rectValue->bottomRightRadius()) {
            Pair* bottomRightRadius = rectValue->bottomRightRadius()->getPairValue();
            rect->setBottomRightRadius(LengthSize(convertToLength(style, rootStyle, bottomRightRadius->first()), convertToLength(style, rootStyle, bottomRightRadius->second())));
        } else
            rect->setBottomRightRadius(LengthSize(Length(0, Fixed), Length(0, Fixed)));

        if (rectValue->topLeftRadius()) {
            Pair* bottomLeftRadius = rectValue->bottomLeftRadius()->getPairValue();
            rect->setBottomLeftRadius(LengthSize(convertToLength(style, rootStyle, bottomLeftRadius->first()), convertToLength(style, rootStyle, bottomLeftRadius->second())));
        } else
            rect->setBottomLeftRadius(LengthSize(Length(0, Fixed), Length(0, Fixed)));

        basicShape = rect.release();
        break;
    }
    default:
        break;
    }

    basicShape->setLayoutBox(boxForValue(basicShapeValue->layoutBox()));

    return basicShape.release();
}

float floatValueForCenterCoordinate(const BasicShapeCenterCoordinate& center, float boxDimension)
{
    float offset = floatValueForLength(center.length(), boxDimension);
    switch (center.keyword()) {
    case BasicShapeCenterCoordinate::None:
        return offset;
    case BasicShapeCenterCoordinate::Top:
    case BasicShapeCenterCoordinate::Left:
        return offset;
    case BasicShapeCenterCoordinate::Bottom:
    case BasicShapeCenterCoordinate::Right:
        return boxDimension - offset;
    }

    ASSERT_NOT_REACHED();
    return 0;
}

}
