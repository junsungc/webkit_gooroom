/*
 * Copyright (C) 2008 Alp Toker <alp@atoker.com>
 * Copyright (C) 2010 Igalia S.L.
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
#include "FontCache.h"

#include "FcUniquePtr.h"
#include "Font.h"
#include "OwnPtrCairo.h"
#include "RefPtrCairo.h"
#include "UTF16UChar32Iterator.h"
#include <cairo-ft.h>
#include <cairo.h>
#include <fontconfig/fcfreetype.h>
#include <wtf/Assertions.h>
#include <wtf/text/CString.h>

namespace WebCore {

void FontCache::platformInit()
{
    // It's fine to call FcInit multiple times per the documentation.
    if (!FcInit())
        ASSERT_NOT_REACHED();
}

FcPattern* createFontConfigPatternForCharacters(const UChar* characters, int bufferLength)
{
    FcPattern* pattern = FcPatternCreate();
    FcCharSet* fontConfigCharSet = FcCharSetCreate();

    UTF16UChar32Iterator iterator(characters, bufferLength);
    UChar32 character = iterator.next();
    while (character != iterator.end()) {
        FcCharSetAddChar(fontConfigCharSet, character);
        character = iterator.next();
    }

    FcPatternAddCharSet(pattern, FC_CHARSET, fontConfigCharSet);
    FcCharSetDestroy(fontConfigCharSet);

    FcPatternAddBool(pattern, FC_SCALABLE, FcTrue);
    FcConfigSubstitute(0, pattern, FcMatchPattern);
    FcDefaultSubstitute(pattern);
    return pattern;
}

FcPattern* findBestFontGivenFallbacks(const FontPlatformData& fontData, FcPattern* pattern)
{
    if (!fontData.m_pattern)
        return 0;

    if (!fontData.m_fallbacks) {
        FcResult fontConfigResult;
        fontData.m_fallbacks = FcFontSort(0, fontData.m_pattern.get(), FcTrue, 0, &fontConfigResult);
    }

    if (!fontData.m_fallbacks)
        return 0;

    FcFontSet* sets[] = { fontData.m_fallbacks };
    FcResult fontConfigResult;
    return FcFontSetMatch(0, sets, 1, pattern, &fontConfigResult);
}

RefPtr<Font> FontCache::systemFallbackForCharacters(const FontDescription& description, const Font* originalFontData, bool, const UChar* characters, int length)
{
    RefPtr<FcPattern> pattern = adoptRef(createFontConfigPatternForCharacters(characters, length));
    const FontPlatformData& fontData = originalFontData->platformData();

    RefPtr<FcPattern> fallbackPattern = adoptRef(findBestFontGivenFallbacks(fontData, pattern.get()));
    if (fallbackPattern) {
        FontPlatformData alternateFontData(fallbackPattern.get(), description);
        return fontForPlatformData(alternateFontData);
    }

    FcResult fontConfigResult;
    RefPtr<FcPattern> resultPattern = adoptRef(FcFontMatch(0, pattern.get(), &fontConfigResult));
    if (!resultPattern)
        return 0;
    FontPlatformData alternateFontData(resultPattern.get(), description);
    return fontForPlatformData(alternateFontData);
}

Ref<Font> FontCache::lastResortFallbackFont(const FontDescription& fontDescription)
{
    // We want to return a fallback font here, otherwise the logic preventing FontConfig
    // matches for non-fallback fonts might return 0. See isFallbackFontAllowed.
    static AtomicString timesStr("serif");
    return *fontForFamily(fontDescription, timesStr, false);
}

void FontCache::getTraitsInFamily(const AtomicString&, Vector<unsigned>&)
{
}

static String getFamilyNameStringFromFamily(const AtomicString& family)
{
    // If we're creating a fallback font (e.g. "-webkit-monospace"), convert the name into
    // the fallback name (like "monospace") that fontconfig understands.
    if (family.length() && !family.startsWith("-webkit-"))
        return family.string();

    if (family == standardFamily || family == serifFamily)
        return "serif";
    if (family == sansSerifFamily)
        return "sans-serif";
    if (family == monospaceFamily)
        return "monospace";
    if (family == cursiveFamily)
        return "cursive";
    if (family == fantasyFamily)
        return "fantasy";
    return "";
}

int fontWeightToFontconfigWeight(FontWeight weight)
{
    switch (weight) {
    case FontWeight100:
        return FC_WEIGHT_THIN;
    case FontWeight200:
        return FC_WEIGHT_ULTRALIGHT;
    case FontWeight300:
        return FC_WEIGHT_LIGHT;
    case FontWeight400:
        return FC_WEIGHT_REGULAR;
    case FontWeight500:
        return FC_WEIGHT_MEDIUM;
    case FontWeight600:
        return FC_WEIGHT_SEMIBOLD;
    case FontWeight700:
        return FC_WEIGHT_BOLD;
    case FontWeight800:
        return FC_WEIGHT_EXTRABOLD;
    case FontWeight900:
        return FC_WEIGHT_ULTRABLACK;
    default:
        ASSERT_NOT_REACHED();
        return FC_WEIGHT_REGULAR;
    }
}

// This is based on Chromium BSD code from Skia (src/ports/SkFontMgr_fontconfig.cpp). It is a
// hack for lack of API in Fontconfig: https://bugs.freedesktop.org/show_bug.cgi?id=19375
// FIXME: This is horrible. It should be deleted once Fontconfig can do this itself.
enum class AliasStrength {
    Weak,
    Strong,
    Done
};

static AliasStrength strengthOfFirstAlias(const FcPattern& original)
{
    // Ideally there would exist a call like
    // FcResult FcPatternIsWeak(pattern, object, id, FcBool* isWeak);
    //
    // However, there is no such call and as of Fc 2.11.0 even FcPatternEquals ignores the weak bit.
    // Currently, the only reliable way of finding the weak bit is by its effect on matching.
    // The weak bit only affects the matching of FC_FAMILY and FC_POSTSCRIPT_NAME object values.
    // A element with the weak bit is scored after FC_LANG, without the weak bit is scored before.
    // Note that the weak bit is stored on the element, not on the value it holds.
    FcValue value;
    FcResult result = FcPatternGet(&original, FC_FAMILY, 0, &value);
    if (result != FcResultMatch)
        return AliasStrength::Done;

    RefPtr<FcPattern> pattern = adoptRef(FcPatternDuplicate(&original));
    FcBool hasMultipleFamilies = true;
    while (hasMultipleFamilies)
        hasMultipleFamilies = FcPatternRemove(pattern.get(), FC_FAMILY, 1);

    // Create a font set with two patterns.
    // 1. the same FC_FAMILY as pattern and a lang object with only 'nomatchlang'.
    // 2. a different FC_FAMILY from pattern and a lang object with only 'matchlang'.
    FcUniquePtr<FcFontSet> fontSet(FcFontSetCreate());

    FcUniquePtr<FcLangSet> strongLangSet(FcLangSetCreate());
    FcLangSetAdd(strongLangSet.get(), reinterpret_cast<const FcChar8*>("nomatchlang"));
    RefPtr<FcPattern> strong = adoptRef(FcPatternDuplicate(pattern.get()));
    FcPatternAddLangSet(strong.get(), FC_LANG, strongLangSet.get());

    FcUniquePtr<FcLangSet> weakLangSet(FcLangSetCreate());
    FcLangSetAdd(weakLangSet.get(), reinterpret_cast<const FcChar8*>("matchlang"));
    RefPtr<FcPattern> weak(FcPatternCreate());
    FcPatternAddString(weak.get(), FC_FAMILY, reinterpret_cast<const FcChar8*>("nomatchstring"));
    FcPatternAddLangSet(weak.get(), FC_LANG, weakLangSet.get());

    FcFontSetAdd(fontSet.get(), strong.leakRef());
    FcFontSetAdd(fontSet.get(), weak.leakRef());

    // Add 'matchlang' to the copy of the pattern.
    FcPatternAddLangSet(pattern.get(), FC_LANG, weakLangSet.get());

    // Run a match against the copy of the pattern.
    // If the first element was weak, then we should match the pattern with 'matchlang'.
    // If the first element was strong, then we should match the pattern with 'nomatchlang'.

    // Note that this config is only used for FcFontRenderPrepare, which we don't even want.
    // However, there appears to be no way to match/sort without it.
    RefPtr<FcConfig> config = adoptRef(FcConfigCreate());
    FcFontSet* fontSets[1] = { fontSet.get() };
    RefPtr<FcPattern> match = adoptRef(FcFontSetMatch(config.get(), fontSets, 1, pattern.get(), &result));

    FcLangSet* matchLangSet;
    FcPatternGetLangSet(match.get(), FC_LANG, 0, &matchLangSet);
    return FcLangEqual == FcLangSetHasLang(matchLangSet, reinterpret_cast<const FcChar8*>("matchlang"))
        ? AliasStrength::Weak : AliasStrength::Strong;
}

static Vector<String> strongAliasesForFamily(const String& family)
{
    RefPtr<FcPattern> pattern = adoptRef(FcPatternCreate());
    if (!FcPatternAddString(pattern.get(), FC_FAMILY, reinterpret_cast<const FcChar8*>(family.utf8().data())))
        return Vector<String>();

    FcConfigSubstitute(nullptr, pattern.get(), FcMatchPattern);
    FcDefaultSubstitute(pattern.get());

    FcUniquePtr<FcObjectSet> familiesOnly(FcObjectSetBuild(FC_FAMILY, nullptr));
    RefPtr<FcPattern> minimal = adoptRef(FcPatternFilter(pattern.get(), familiesOnly.get()));

    // We really want to match strong (preferred) and same (acceptable) only here.
    // If a family name was specified, assume that any weak matches after the last strong match
    // are weak (default) and ignore them.
    // The reason for is that after substitution the pattern for 'sans-serif' looks like
    // "wwwwwwwwwwwwwwswww" where there are many weak but preferred names, followed by defaults.
    // So it is possible to have weakly matching but preferred names.
    // In aliases, bindings are weak by default, so this is easy and common.
    // If no family name was specified, we'll probably only get weak matches, but that's ok.
    int lastStrongId = -1;
    int numIds = 0;
    for (int id = 0; ; ++id) {
        AliasStrength result = strengthOfFirstAlias(*minimal);
        if (result == AliasStrength::Done) {
            numIds = id;
            break;
        }
        if (result == AliasStrength::Strong)
            lastStrongId = id;
        if (!FcPatternRemove(minimal.get(), FC_FAMILY, 0))
            return Vector<String>();
    }

    // If they were all weak, then leave the pattern alone.
    if (lastStrongId < 0)
        return Vector<String>();

    // Remove everything after the last strong.
    for (int id = lastStrongId + 1; id < numIds; ++id) {
        if (!FcPatternRemove(pattern.get(), FC_FAMILY, lastStrongId + 1)) {
            ASSERT_NOT_REACHED();
            return Vector<String>();
        }
    }

    // Take the resulting pattern and remove everything but the families.
    minimal = adoptRef(FcPatternFilter(pattern.get(), familiesOnly.get()));
    // Convert the pattern to a string, and cut out the non-family junk that gets added to the end.
    char* patternChars = reinterpret_cast<char*>(FcPatternFormat(pattern.get(), reinterpret_cast<const FcChar8*>("%{family}")));
    String patternString = String::fromUTF8(patternChars);
    free(patternChars);

    Vector<String> results;
    patternString.split(',', results);
    return results;
}

static bool areStronglyAliased(const String& familyA, const String& familyB)
{
    for (auto& family : strongAliasesForFamily(familyA)) {
        if (family == familyB)
            return true;
    }
    return false;
}


std::unique_ptr<FontPlatformData> FontCache::createFontPlatformData(const FontDescription& fontDescription, const AtomicString& family)
{
    // The CSS font matching algorithm (http://www.w3.org/TR/css3-fonts/#font-matching-algorithm)
    // says that we must find an exact match for font family, slant (italic or oblique can be used)
    // and font weight (we only match bold/non-bold here).
    RefPtr<FcPattern> pattern = adoptRef(FcPatternCreate());
    // Never choose unscalable fonts, as they pixelate when displayed at different sizes.
    FcPatternAddBool(pattern.get(), FC_SCALABLE, FcTrue);
    String familyNameString(getFamilyNameStringFromFamily(family));
    if (!FcPatternAddString(pattern.get(), FC_FAMILY, reinterpret_cast<const FcChar8*>(familyNameString.utf8().data())))
        return nullptr;

    bool italic = fontDescription.italic();
    if (!FcPatternAddInteger(pattern.get(), FC_SLANT, italic ? FC_SLANT_ITALIC : FC_SLANT_ROMAN))
        return nullptr;
    if (!FcPatternAddInteger(pattern.get(), FC_WEIGHT, fontWeightToFontconfigWeight(fontDescription.weight())))
        return nullptr;
    if (!FcPatternAddDouble(pattern.get(), FC_PIXEL_SIZE, fontDescription.computedPixelSize()))
        return nullptr;

    // The strategy is originally from Skia (src/ports/SkFontHost_fontconfig.cpp):
    //
    // We do not normally allow fontconfig to substitute one font family for another, since this
    // would break CSS font family fallback: the website should be in control of fallback. During
    // normal font matching, the only font family substitution permitted is for generic families
    // (sans, serif, monospace) or for strongly-aliased fonts (which are to be treated as
    // effectively identical). This is because the font matching step is designed to always find a
    // match for the font, which we don't want.
    //
    // Fontconfig is used in two stages: (1) configuration and (2) matching. During the
    // configuration step, before any matching occurs, we allow arbitrary family substitutions,
    // since this is an exact matter of respecting the user's font configuration.
    FcConfigSubstitute(0, pattern.get(), FcMatchPattern);
    FcDefaultSubstitute(pattern.get());

    FcChar8* fontConfigFamilyNameAfterConfiguration;
    FcPatternGetString(pattern.get(), FC_FAMILY, 0, &fontConfigFamilyNameAfterConfiguration);
    String familyNameAfterConfiguration = String::fromUTF8(reinterpret_cast<char*>(fontConfigFamilyNameAfterConfiguration));

    FcResult fontConfigResult;
    RefPtr<FcPattern> resultPattern = adoptRef(FcFontMatch(0, pattern.get(), &fontConfigResult));
    if (!resultPattern) // No match.
        return nullptr;

    FcChar8* fontConfigFamilyNameAfterMatching;
    FcPatternGetString(resultPattern.get(), FC_FAMILY, 0, &fontConfigFamilyNameAfterMatching);
    String familyNameAfterMatching = String::fromUTF8(reinterpret_cast<char*>(fontConfigFamilyNameAfterMatching));

    // If Fontconfig gave us a different font family than the one we requested, we should ignore it
    // and allow WebCore to give us the next font on the CSS fallback list. The exceptions are if
    // this family name is a commonly-used generic family, or if the families are strongly-aliased.
    // Checking for a strong alias comes last, since it is slow.
    if (!equalIgnoringCase(familyNameAfterConfiguration, familyNameAfterMatching)
        && !(equalIgnoringCase(familyNameString, "sans") || equalIgnoringCase(familyNameString, "sans-serif")
          || equalIgnoringCase(familyNameString, "serif") || equalIgnoringCase(familyNameString, "monospace")
          || equalIgnoringCase(familyNameString, "fantasy") || equalIgnoringCase(familyNameString, "cursive"))
        && !areStronglyAliased(familyNameAfterConfiguration, familyNameAfterMatching))
        return nullptr;

    // Verify that this font has an encoding compatible with Fontconfig. Fontconfig currently
    // supports three encodings in FcFreeTypeCharIndex: Unicode, Symbol and AppleRoman.
    // If this font doesn't have one of these three encodings, don't select it.
    auto platformData = std::make_unique<FontPlatformData>(resultPattern.get(), fontDescription);
    if (!platformData->hasCompatibleCharmap())
        return nullptr;

    return platformData;
}

}
