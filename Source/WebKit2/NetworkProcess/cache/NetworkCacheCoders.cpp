/*
 * Copyright (C) 2011, 2014-2015 Apple Inc. All rights reserved.
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
#include "NetworkCacheCoders.h"

#if ENABLE(NETWORK_CACHE)

#include "WebCoreArgumentCoders.h"
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>

namespace WebKit {

void NetworkCacheCoder<AtomicString>::encode(NetworkCacheEncoder& encoder, const AtomicString& atomicString)
{
    encoder << atomicString.string();
}

bool NetworkCacheCoder<AtomicString>::decode(NetworkCacheDecoder& decoder, AtomicString& atomicString)
{
    String string;
    if (!decoder.decode(string))
        return false;

    atomicString = string;
    return true;
}

void NetworkCacheCoder<CString>::encode(NetworkCacheEncoder& encoder, const CString& string)
{
    // Special case the null string.
    if (string.isNull()) {
        encoder << std::numeric_limits<uint32_t>::max();
        return;
    }

    uint32_t length = string.length();
    encoder << length;
    encoder.encodeFixedLengthData(reinterpret_cast<const uint8_t*>(string.data()), length);
}

bool NetworkCacheCoder<CString>::decode(NetworkCacheDecoder& decoder, CString& result)
{
    uint32_t length;
    if (!decoder.decode(length))
        return false;

    if (length == std::numeric_limits<uint32_t>::max()) {
        // This is the null string.
        result = CString();
        return true;
    }

    // Before allocating the string, make sure that the decoder buffer is big enough.
    if (!decoder.bufferIsLargeEnoughToContain<char>(length)) {
        decoder.markInvalid();
        return false;
    }

    char* buffer;
    CString string = CString::newUninitialized(length, buffer);
    if (!decoder.decodeFixedLengthData(reinterpret_cast<uint8_t*>(buffer), length))
        return false;

    result = string;
    return true;
}


void NetworkCacheCoder<String>::encode(NetworkCacheEncoder& encoder, const String& string)
{
    // Special case the null string.
    if (string.isNull()) {
        encoder << std::numeric_limits<uint32_t>::max();
        return;
    }

    uint32_t length = string.length();
    bool is8Bit = string.is8Bit();

    encoder << length << is8Bit;

    if (is8Bit)
        encoder.encodeFixedLengthData(reinterpret_cast<const uint8_t*>(string.characters8()), length * sizeof(LChar));
    else
        encoder.encodeFixedLengthData(reinterpret_cast<const uint8_t*>(string.characters16()), length * sizeof(UChar));
}

template <typename CharacterType>
static inline bool decodeStringText(NetworkCacheDecoder& decoder, uint32_t length, String& result)
{
    // Before allocating the string, make sure that the decoder buffer is big enough.
    if (!decoder.bufferIsLargeEnoughToContain<CharacterType>(length)) {
        decoder.markInvalid();
        return false;
    }
    
    CharacterType* buffer;
    String string = String::createUninitialized(length, buffer);
    if (!decoder.decodeFixedLengthData(reinterpret_cast<uint8_t*>(buffer), length * sizeof(CharacterType)))
        return false;
    
    result = string;
    return true;    
}

bool NetworkCacheCoder<String>::decode(NetworkCacheDecoder& decoder, String& result)
{
    uint32_t length;
    if (!decoder.decode(length))
        return false;

    if (length == std::numeric_limits<uint32_t>::max()) {
        // This is the null string.
        result = String();
        return true;
    }

    bool is8Bit;
    if (!decoder.decode(is8Bit))
        return false;

    if (is8Bit)
        return decodeStringText<LChar>(decoder, length, result);
    return decodeStringText<UChar>(decoder, length, result);
}

void NetworkCacheCoder<WebCore::CertificateInfo>::encode(NetworkCacheEncoder& encoder, const WebCore::CertificateInfo& certificateInfo)
{
    // FIXME: Cocoa CertificateInfo is a CF object tree. Generalize CF type coding so we don't need to use ArgumentCoder here.
    IPC::ArgumentEncoder argumentEncoder;
    argumentEncoder << certificateInfo;
    encoder << static_cast<uint64_t>(argumentEncoder.bufferSize());
    encoder.encodeFixedLengthData(argumentEncoder.buffer(), argumentEncoder.bufferSize());
}

bool NetworkCacheCoder<WebCore::CertificateInfo>::decode(NetworkCacheDecoder& decoder, WebCore::CertificateInfo& certificateInfo)
{
    uint64_t certificateSize;
    if (!decoder.decode(certificateSize))
        return false;
    Vector<uint8_t> data(certificateSize);
    if (!decoder.decodeFixedLengthData(data.data(), data.size()))
        return false;
    IPC::ArgumentDecoder argumentDecoder(data.data(), data.size());
    if (!argumentDecoder.decode(certificateInfo)) {
        decoder.markInvalid();
        return false;
    }
    return true;
}

void NetworkCacheCoder<MD5::Digest>::encode(NetworkCacheEncoder& encoder, const MD5::Digest& digest)
{
    encoder.encodeFixedLengthData(digest.data(), sizeof(digest));
}

bool NetworkCacheCoder<MD5::Digest>::decode(NetworkCacheDecoder& decoder, MD5::Digest& digest)
{
    return decoder.decodeFixedLengthData(digest.data(), sizeof(digest));
}

}

#endif