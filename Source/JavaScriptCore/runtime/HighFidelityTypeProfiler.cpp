/*
 * Copyright (C) 2014 Apple Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "HighFidelityTypeProfiler.h"

#include "InspectorJSTypeBuilders.h"
#include "TypeLocation.h"

namespace JSC {

static const bool verbose = false;

void HighFidelityTypeProfiler::logTypesForTypeLocation(TypeLocation* location)
{
    TypeProfilerSearchDescriptor descriptor = location->m_globalVariableID == HighFidelityReturnStatement ? TypeProfilerSearchDescriptorFunctionReturn : TypeProfilerSearchDescriptorNormal;

    dataLogF("[Start, End]::[%u, %u]\n", location->m_divotStart, location->m_divotEnd);

    if (findLocation(location->m_divotStart, location->m_sourceID, descriptor))
        dataLog("\t\t[Entry IS in System]\n");
    else
        dataLog("\t\t[Entry IS NOT in system]\n");

    dataLog("\t\t", location->m_globalVariableID == HighFidelityReturnStatement ? "[Return Statement]" : "[Normal Statement]", "\n");

    dataLog("\t\t#Local#\n\t\t", location->m_instructionTypeSet->seenTypes().replace("\n", "\n\t\t"), "\n");
    if (location->m_globalTypeSet)
        dataLog("\t\t#Global#\n\t\t", location->m_globalTypeSet->seenTypes().replace("\n", "\n\t\t"), "\n");
}

void HighFidelityTypeProfiler::insertNewLocation(TypeLocation* location)
{
    if (verbose)
        dataLogF("Registering location:: divotStart:%u, divotEnd:%u\n", location->m_divotStart, location->m_divotEnd);

    if (!m_bucketMap.contains(location->m_sourceID)) {
        Vector<TypeLocation*> bucket;
        m_bucketMap.set(location->m_sourceID, bucket);
    }

    Vector<TypeLocation*>& bucket = m_bucketMap.find(location->m_sourceID)->value;
    bucket.append(location);
}

void HighFidelityTypeProfiler::getTypesForVariableAtOffsetForInspector(TypeProfilerSearchDescriptor descriptor, unsigned divot, intptr_t sourceID, RefPtr<Inspector::TypeBuilder::Runtime::TypeDescription>& description)
{
    TypeLocation* location = findLocation(divot, sourceID, descriptor);
    if (!location)
        return;

    if (location->m_globalTypeSet && location->m_globalVariableID != HighFidelityNoGlobalIDExists) {
        description->setDisplayTypeName(location->m_globalTypeSet->displayName());
        description->setGlobalPrimitiveTypeNames(location->m_globalTypeSet->allPrimitiveTypeNames());
        description->setGlobalStructures(location->m_globalTypeSet->allStructureRepresentations());
    } else
        description->setDisplayTypeName(location->m_instructionTypeSet->displayName());

    description->setLocalPrimitiveTypeNames(location->m_instructionTypeSet->allPrimitiveTypeNames());
    description->setLocalStructures(location->m_instructionTypeSet->allStructureRepresentations());
}

static bool descriptorMatchesTypeLocation(TypeProfilerSearchDescriptor descriptor, TypeLocation* location)
{
    if (descriptor == TypeProfilerSearchDescriptorFunctionReturn && location->m_globalVariableID == HighFidelityReturnStatement)  
        return true;

    if (descriptor == TypeProfilerSearchDescriptorNormal && location->m_globalVariableID != HighFidelityReturnStatement)  
        return true;

    return false;
}

TypeLocation* HighFidelityTypeProfiler::findLocation(unsigned divot, intptr_t sourceID, TypeProfilerSearchDescriptor descriptor)
{
    QueryKey queryKey(sourceID, divot);
    auto iter = m_queryCache.find(queryKey);
    if (iter != m_queryCache.end())
        return iter->value;

    if (!m_functionHasExecutedCache.hasExecutedAtOffset(sourceID, divot))
        return nullptr;

    ASSERT(m_bucketMap.contains(sourceID));

    Vector<TypeLocation*>& bucket = m_bucketMap.find(sourceID)->value;
    TypeLocation* bestMatch = nullptr;
    unsigned distance = UINT_MAX; // Because assignments may be nested, make sure we find the closest enclosing assignment to this character offset.
    for (size_t i = 0, size = bucket.size(); i < size; i++) {
        TypeLocation* location = bucket.at(i);
        if (descriptor == TypeProfilerSearchDescriptorFunctionReturn && descriptorMatchesTypeLocation(descriptor, location) && location->m_divotForFunctionOffsetIfReturnStatement == divot)
            return location;

        if (location->m_divotStart <= divot && divot <= location->m_divotEnd && location->m_divotEnd - location->m_divotStart <= distance && descriptorMatchesTypeLocation(descriptor, location)) {
            distance = location->m_divotEnd - location->m_divotStart;
            bestMatch = location;
        }
    }

    if (bestMatch)
        m_queryCache.set(queryKey, bestMatch);
    // FIXME: BestMatch should never be null past this point. This doesn't hold currently because we ignore var assignments when code contains eval/With (VarInjection). 
    // https://bugs.webkit.org/show_bug.cgi?id=135184
    return bestMatch;
}

} //namespace JSC
