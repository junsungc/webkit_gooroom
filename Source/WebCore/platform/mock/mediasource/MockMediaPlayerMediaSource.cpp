/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "MockMediaPlayerMediaSource.h"

#if ENABLE(MEDIA_SOURCE)

#include "ExceptionCodePlaceholder.h"
#include "HTMLMediaSource.h"
#include "MediaPlayer.h"
#include "MockMediaSourcePrivate.h"
#include <wtf/Functional.h>
#include <wtf/MainThread.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

// MediaPlayer Enigne Support
void MockMediaPlayerMediaSource::registerMediaEngine(MediaEngineRegistrar registrar)
{
    registrar(create, getSupportedTypes, supportsType, 0, 0, 0);
}

PassOwnPtr<MediaPlayerPrivateInterface> MockMediaPlayerMediaSource::create(MediaPlayer* player)
{
    return adoptPtr(new MockMediaPlayerMediaSource(player));
}

static HashSet<String> mimeTypeCache()
{
    DEFINE_STATIC_LOCAL(HashSet<String>, cache, ());
    static bool isInitialized = false;

    if (!isInitialized) {
        isInitialized = true;
        cache.add(ASCIILiteral("video/mock"));
    }

    return cache;
}

void MockMediaPlayerMediaSource::getSupportedTypes(HashSet<String>& supportedTypes)
{
    supportedTypes = mimeTypeCache();
}

MediaPlayer::SupportsType MockMediaPlayerMediaSource::supportsType(const MediaEngineSupportParameters& parameters)
{
    if (!parameters.isMediaSource)
        return MediaPlayer::IsNotSupported;

    if (!mimeTypeCache().contains(parameters.type))
        return MediaPlayer::IsNotSupported;

    if (parameters.codecs.isEmpty())
        return MediaPlayer::MayBeSupported;

    return parameters.codecs == "mock" ? MediaPlayer::IsSupported : MediaPlayer::MayBeSupported;
}

MockMediaPlayerMediaSource::MockMediaPlayerMediaSource(MediaPlayer* player)
    : m_player(player)
    , m_currentTime(0)
    , m_duration(0)
    , m_readyState(MediaPlayer::HaveNothing)
    , m_networkState(MediaPlayer::Empty)
    , m_playing(false)
{
}

MockMediaPlayerMediaSource::~MockMediaPlayerMediaSource()
{
}

void MockMediaPlayerMediaSource::load(const String&)
{
    ASSERT_NOT_REACHED();
}

void MockMediaPlayerMediaSource::load(const String&, PassRefPtr<HTMLMediaSource> source)
{
    m_mediaSource = source;
    m_mediaSourcePrivate = MockMediaSourcePrivate::create(this);
    m_mediaSource->setPrivateAndOpen(*m_mediaSourcePrivate);
}

void MockMediaPlayerMediaSource::cancelLoad()
{
}

void MockMediaPlayerMediaSource::play()
{
    m_playing = 1;
    callOnMainThread(bind(&MockMediaPlayerMediaSource::advanceCurrentTime, this));
}

void MockMediaPlayerMediaSource::pause()
{
    m_playing = 0;
}

IntSize MockMediaPlayerMediaSource::naturalSize() const
{
    return IntSize();
}

bool MockMediaPlayerMediaSource::hasVideo() const
{
    return m_mediaSourcePrivate ? m_mediaSourcePrivate->hasVideo() : false;
}

bool MockMediaPlayerMediaSource::hasAudio() const
{
    return m_mediaSourcePrivate ? m_mediaSourcePrivate->hasAudio() : false;
}

void MockMediaPlayerMediaSource::setVisible(bool)
{
}

bool MockMediaPlayerMediaSource::seeking() const
{
    return false;
}

bool MockMediaPlayerMediaSource::paused() const
{
    return !m_playing;
}

MediaPlayer::NetworkState MockMediaPlayerMediaSource::networkState() const
{
    return m_networkState;
}

MediaPlayer::ReadyState MockMediaPlayerMediaSource::readyState() const
{
    return m_readyState;
}

PassRefPtr<TimeRanges> MockMediaPlayerMediaSource::buffered() const
{
    return m_mediaSource ? m_mediaSource->buffered() : TimeRanges::create();
}

bool MockMediaPlayerMediaSource::didLoadingProgress() const
{
    return false;
}

void MockMediaPlayerMediaSource::setSize(const IntSize&)
{
}

void MockMediaPlayerMediaSource::paint(GraphicsContext*, const IntRect&)
{
}

double MockMediaPlayerMediaSource::currentTimeDouble() const
{
    return m_currentTime;
}

double MockMediaPlayerMediaSource::durationDouble() const
{
    return m_duration;
}

void MockMediaPlayerMediaSource::seekDouble(double time)
{
    m_currentTime = std::min(time, m_duration);
    m_player->timeChanged();

    if (m_playing)
        callOnMainThread(bind(&MockMediaPlayerMediaSource::advanceCurrentTime, this));
}

void MockMediaPlayerMediaSource::advanceCurrentTime()
{
    RefPtr<TimeRanges> buffered = this->buffered();
    size_t pos = buffered->find(m_currentTime);
    if (pos == notFound)
        return;

    m_currentTime = std::min(m_duration, buffered->end(pos, IGNORE_EXCEPTION));
    m_player->timeChanged();
}

void MockMediaPlayerMediaSource::updateDuration(double duration)
{
    if (m_duration == duration)
        return;

    m_duration = duration;
    m_player->durationChanged();
}

void MockMediaPlayerMediaSource::setReadyState(MediaPlayer::ReadyState readyState)
{
    if (readyState == m_readyState)
        return;

    m_readyState = readyState;
    m_player->readyStateChanged();
}

}

#endif
