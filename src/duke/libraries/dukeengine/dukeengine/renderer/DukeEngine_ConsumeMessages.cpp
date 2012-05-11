/*
 * DukeEngineConsumeMessages.cpp
 *
 *  Created on: May 7, 2012
 *      Author: Guillaume Chatelet
 */

#include "DukeEngine.h"

using namespace ::duke::protocol;
using namespace ::google::protobuf;
using namespace ::google::protobuf::serialize;

static const string HEADER = "[DukeEngine] ";

////////////////////////////////////
// State Messages Routing
////////////////////////////////////

static inline void dump(const google::protobuf::Descriptor* pDescriptor, const google::protobuf::serialize::MessageHolder &holder, bool push = false) {
#ifdef true //DEBUG_MESSAGES
    const string debugString = pDescriptor == Texture::descriptor() ? "texture" : unpack(holder)->ShortDebugString();
    cerr << HEADER + (push ? "push " : "pop  ") + pDescriptor->name() << "\t" << debugString << endl;
#endif
}

void DukeEngine::consumeStateMessages() {
    SharedHolder pHolder;
    while (m_IO.tryPop(pHolder)) {
        if (!pHolder)
            throw quit_exception(EXIT_FAILURE, "NULL message in DukeEngine");
        if (pHolder->action() == MessageHolder::CLOSE_CONNECTION)
            throw quit_exception(pHolder->return_value(), "Connection closed by peer");
        const MessageHolder &holder = *pHolder;
        const Descriptor* pDescriptor = descriptorFor(holder);
        dump(pDescriptor, holder);
        if (isType<duke::protocol::Renderer>(pDescriptor))
            cerr << HEADER + "calling INIT_RENDERER twice is forbidden" << endl;
        else if (isType<Debug>(pDescriptor))
            consumeDebug(unpackTo<Debug>(holder));
        else if (isType<Scene>(pDescriptor))
            consumeScene(unpackTo<Scene>(holder));
        else if (isType<Transport>(pDescriptor))
            consumeTransport(unpackTo<Transport>(holder), holder.action());
        else if (isType<PlaybackState>(pDescriptor))
            consumePlaybackState(unpackTo<PlaybackState>(holder));
        else if (isType<Info>(pDescriptor))
            consumeInfo(unpackTo<Info>(holder), holder.action());
//        else if (isType<Cache>(pDescriptor))
//            consumeCache(unpackTo<Cache>(holder), holder.action());
        else
            m_RenderQueue.push(pHolder);
    }
}

////////////////////////////////////
// Scene
////////////////////////////////////

void DukeEngine::consumeScene(const Scene& scene) {
    m_Playlist = PlaylistHelper(scene);
    m_AudioEngine.load(scene);
    updatePlaybackState();
    m_Cache.init(m_Playlist, m_CacheConfiguration);
    m_bForceRefresh = true;
}

////////////////////////////////////
// Transport
////////////////////////////////////

void DukeEngine::consumeTransport(const Transport &transport, const MessageHolder::Action action) {
    switch (action) {
        case MessageHolder::CREATE: {
            applyTransport(transport);
            if (transport.has_autonotifyonframechange())
                m_bAutoNotifyOnFrameChange = transport.autonotifyonframechange();
            if (transport.has_dorender() && transport.dorender())
                return;
            break;
        }
        case MessageHolder::RETRIEVE: {
            Transport transport;
            transport.set_type(::Transport::CUE);
            Transport_Cue *cue = transport.mutable_cue();
            cue->set_value(m_Playback.frame());
            push(packAndShare(transport));
            break;
        }
        default: {
            cerr << HEADER + "unknown action for transport message " << MessageHolder_Action_Name(action) << endl;
            break;
        }
    }
}

////////////////////////////////////
// Playback State
////////////////////////////////////

void DukeEngine::consumePlaybackState(const PlaybackState &playbackState) {
    // update playback
    bool changed = false;
    if (playbackState.has_frameratenumerator()) {
        m_PlaybackState.set_frameratenumerator(playbackState.frameratenumerator());
        if (playbackState.has_frameratedenominator())
            m_PlaybackState.set_frameratedenominator(playbackState.frameratedenominator());
        changed = true;
    }
    if (playbackState.has_loop()) {
        m_PlaybackState.set_loop(playbackState.loop());
        changed = true;
    }
    if (playbackState.has_playbackmode()) {
        m_PlaybackState.set_playbackmode(playbackState.playbackmode());
        changed = true;
    }
    if (changed)
        updatePlaybackState();
}

static inline playback::PlaybackType convert(PlaybackState::PlaybackMode mode) {
    switch (mode) {
        case PlaybackState::RENDER:
            return playback::RENDER;
        case PlaybackState::NO_SKIP:
            return playback::REALTIME_NO_SKIP;
        case PlaybackState::DROP_FRAME_TO_KEEP_REALTIME:
            return playback::REALTIME;
        default:
            throw runtime_error("bad enum");
    }
}

void DukeEngine::updatePlaybackState() {
    using namespace boost::chrono;
    const high_resolution_clock::duration nsPerFrame = playback::nsPerFrame(m_PlaybackState.frameratenumerator(), m_PlaybackState.frameratedenominator());
    m_Playback.init(m_Playlist.range, m_PlaybackState.loop(), nsPerFrame);
    m_Playback.setType(convert(m_PlaybackState.playbackmode()));
}

////////////////////////////////////
// Dump
////////////////////////////////////

struct FilenameExtractor {
    const image::WorkUnitId& id;
    FilenameExtractor(const image::WorkUnitId& id) :
                    id(id) {
    }
};

ostream& operator<<(ostream& stream, const FilenameExtractor& fe) {
    return stream << fe.id.filename;
}

string DukeEngine::dumpInfo(const Debug::Content& info) const {
    stringstream ss;

    switch (info) {
        case Debug::FRAME:
            ss << m_Playback.frame();
            break;
        case Debug::FILENAMES: {
            image::WorkUnitIds ids;
            ids.reserve(200);
            m_Cache.dumpKeys(ids);
            copy(ids.begin(), ids.end(), ostream_iterator<FilenameExtractor>(ss, "\n"));
//            ss << "found " << ids.size() << " files in cache";
            break;
        }
        case Debug::FPS: {
            ss << m_FrameTimings.frequency();
            break;
        }
    }
    return ss.str();
}

void DukeEngine::consumeDebug(const Debug &debug) const {
#ifdef __linux__
    cout << "\e[J";
#endif
    for (int i = 0; i < debug.line_size(); ++i) {
        size_t found;
        string line = debug.line(i);
        found = line.find_first_of("%");

        while (found != string::npos) {
            stringstream ss;
            ss << line[found + 1];
            int contentID = atoi(ss.str().c_str());
            if (contentID < debug.content_size())
                line.replace(found, 2, dumpInfo(debug.content(contentID)));
            found = line.find_first_of("%", found + 1);
        }
        cout << line << endl;
    }
#ifdef __linux__
    stringstream ss;
    ss << "\r\e[" << debug.line_size() + 1 << "A";
    cout << ss.str() << endl;
#endif
    if (debug.has_pause())
        ::boost::this_thread::sleep(::boost::posix_time::seconds(debug.pause()));
}

////////////////////////////////////
// Info
////////////////////////////////////

#include <dukeengine/renderer/utils/PixelFormatUtils.h>

#include <sequence/parser/details/Utils.h>

#include <boost/container/flat_set.hpp>

struct CacheStateGatherer {
    typedef boost::container::flat_set<unsigned int> IndexSet;
    const PlaylistHelper &playlist;
    vector<IndexSet> frames;
    Info_CacheState &cache;
    CacheStateGatherer(const PlaylistHelper &playlist, Info_CacheState &cache) :
                    playlist(playlist), frames(playlist.tracks.size()), cache(cache) {
    }
    void ingest(const image::WorkUnitIds &ids) {
        for (image::WorkUnitIds::const_iterator itr = ids.begin(); itr != ids.end(); ++itr)
            frames[itr->index.track].insert(itr->index.frame);
    }
    void update() const {
        for (size_t i = 0; i < frames.size(); ++i) {
            Info_CacheState_TrackCache &trackCache = *cache.add_track();
            trackCache.set_name(playlist.scene.track(i).name());
            size_t step = 0;
            const Ranges ranges = sequence::parser::details::getRangesAndStep(frames[i].begin(), frames[i].end(), step);
            for (Ranges::const_iterator itr = ranges.begin(); itr != ranges.end(); ++itr) {
                FrameRange &range = *trackCache.add_range();
                range.set_first(itr->first);
                range.set_last(itr->last);
            }
        }
    }
};

void DukeEngine::updateInfoCacheState(Info::CacheState &infos) const {
    if (!m_Cache.enabled())
        return;
    image::WorkUnitIds ids;
    infos.set_ram(m_Cache.dumpKeys(ids));
    CacheStateGatherer gatherer(m_Playlist, infos);
    gatherer.ingest(ids);
    gatherer.update();
}

void DukeEngine::updateInfoPlaybackState(Info::PlaybackState &infos) const {
    infos.set_frame(m_Playback.frame());
    infos.set_fps(m_FrameTimings.frequency());
}

void DukeEngine::updateInfoImages(RepeatedPtrField<Info::ImageInfo> &infos) const {
    if (m_FileBufferHolder.getImages().empty())
        return;
    MediaFrames frames;
    m_Playlist.mediaFramesAt(m_Playback.frame(), frames);
    for (MediaFrames::const_iterator itr = frames.begin(); itr != frames.end(); ++itr) {
        const unsigned track = itr->index.track;
        const ImageHolder &holder = m_FileBufferHolder.getImages()[track];
        const ImageDescription &description = holder.getImageDescription();
        Info_ImageInfo &current = *infos.Add();
        current.set_filename(itr->filename());
        current.set_track(track);
        current.set_width(description.width);
        current.set_height(description.height);
        current.set_depth(description.depth);
        current.set_format(toString(description.format));
        current.set_imagesize(description.imageDataSize);
        current.set_filesize(description.fileDataSize);
    }
}

void DukeEngine::updateInfoExtensions(RepeatedPtrField<string> &extensions) const {
    const char ** pExtensions = m_ImageDecoderFactory.getAvailableExtensions();
    for (; pExtensions != NULL && *pExtensions != NULL; ++pExtensions)
        *extensions.Add() = *pExtensions;
}

void DukeEngine::consumeInfo(Info info, const MessageHolder::Action action) {
    switch (info.content()) {
        case Info::PLAYBACKSTATE:
            updateInfoPlaybackState(*info.mutable_playbackstate());
            break;
        case Info::CACHESTATE:
            updateInfoCacheState(*info.mutable_cachestate());
            break;
        case Info::IMAGEINFO:
            updateInfoImages(*info.mutable_image());
            break;
        case Info::EXTENSIONS:
            updateInfoExtensions(*info.mutable_extension());
            break;
        default:
            return;
    }
    switch (action) {
        case MessageHolder::CREATE:
            info.PrintDebugString();
            break;
        case MessageHolder::RETRIEVE: {
            push(packAndShare(info));
            break;
        }
        default:
            break;
    }
}
