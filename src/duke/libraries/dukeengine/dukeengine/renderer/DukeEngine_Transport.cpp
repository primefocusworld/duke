/*
 * DukeEngineTransport.cpp
 *
 *  Created on: May 7, 2012
 *      Author: Guillaume Chatelet
 */

#include "DukeEngine.h"

using namespace ::duke::protocol;
using namespace ::google::protobuf;
using namespace ::google::protobuf::serialize;

static inline uint32_t cueClipRelative(const PlaylistHelper &helper, unsigned int currentFrame, int clipOffset) {
    const Ranges & clips = helper.allClips;
    if (clips.empty())
        return currentFrame;
    const Ranges::const_iterator itr = find_if(clips.begin(), clips.end(), boost::bind(&sequence::Range::contains, _1, currentFrame));
    assert(itr!=clips.end());
    const size_t index = distance(clips.begin(), itr);
    const int newIndex = int(index) + clipOffset;
    const int boundIndex = std::max(0, std::min(int(clips.size()) - 1, newIndex));
    return clips[boundIndex].first;
}

static inline uint32_t cueClipAbsolute(const PlaylistHelper &helper, unsigned int currentFrame, unsigned clipIndex) {
    const Ranges & clips = helper.allClips;
    if (clipIndex >= clips.size()) {
        cerr << "Can't cue to clip " << clipIndex << ", there is only " << clips.size() << " clips" << endl;
        return currentFrame;
    }
    return clips[clipIndex].first;
}

static inline uint32_t cueClip(const Transport::Cue& cue, const PlaylistHelper &helper, unsigned int current) {
    return cue.cuerelative() ? cueClipRelative(helper, current, cue.value()) : cueClipAbsolute(helper, current, cue.value());
}

static inline uint32_t cueFrame(const Transport::Cue& cue, const PlaylistHelper &helper, int32_t current) {
    if (cue.cuerelative())
        return helper.range.offsetLoopFrame(current, cue.value()).first;
    else
        return helper.range.clampFrame(cue.value());
}

static inline uint32_t getFrameFromCueMessage(const Transport_Cue& cue, const PlaylistHelper &helper, int32_t current) {
    return cue.cueclip() ? cueClip(cue, helper, current) : cueFrame(cue, helper, current);
}

void DukeEngine::applyTransport(const Transport& transport) {
    //                        m_AudioEngine.applyTransport(transport);
    const uint32_t currentFrame = m_Playback.frame();
    switch (transport.type()) {
        case Transport::PLAY:
            m_Playback.play(currentFrame, 1);
            m_AudioEngine.sync(m_Playback.playlistTime());
            m_AudioEngine.play();
            break;
        case Transport::STOP:
            m_Playback.stop();
            m_AudioEngine.pause();
            break;
        case Transport::STORE:
            m_StoredFrame = currentFrame;
            break;
        case Transport::CUE:
            m_Playback.cue(getFrameFromCueMessage(transport.cue(), m_Playlist, currentFrame));
            m_AudioEngine.pause();
            break;
        case Transport::CUE_FIRST:
            m_Playback.cue(m_Playlist.range.first);
            m_AudioEngine.pause();
            break;
        case Transport::CUE_LAST:
            m_Playback.cue(m_Playlist.range.last);
            m_AudioEngine.pause();
            break;
        case Transport::CUE_STORED:
            m_Playback.cue(m_StoredFrame);
            m_AudioEngine.pause();
            break;
    }
}
