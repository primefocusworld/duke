/*
 * EngineState.h
 *
 *  Created on: May 2, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef DUKE_ENGINE_H_
#define DUKE_ENGINE_H_

#include <dukeengine/time_statistics/Durations.h>
#include <dukeengine/audio/AudioEngine.h>
#include <dukeengine/playback/Playback.h>
#include <dukeengine/playback/Timings.h>
#include <dukeengine/image/SmartCache.h>
#include <dukeengine/image/FileBufferHolder.h>
#include <dukeengine/host/io/ImageDecoderFactoryImpl.h>

#include <protocol.pb.h>

#include <dukeapi/sequence/PlaylistHelper.h>
#include <dukeapi/MessageQueue.h>

#include <boost/noncopyable.hpp>

#include <vector>

// fwd declaration
class IRenderer;
class DukeWidget;
namespace google {namespace protobuf {namespace serialize {class MessageHolder;}}}

class IDukeEngine : public boost::noncopyable {
public:
    virtual ~IDukeEngine() = 0;
    virtual void onUpdate(DukeWidget&) = 0;
    virtual void onEvent(const google::protobuf::serialize::MessageHolder&) = 0;
};

// fwd declaration
namespace duke {namespace protocol {struct PlaylistHelper;}}
namespace google {namespace protobuf {class Message;}}

class DukeEngine : public IDukeEngine {
public:
    DukeEngine(ImageDecoderFactory& imageDecoderFactory, const duke::protocol::Cache& cacheConfiguration, IMessageIO &);
    virtual ~DukeEngine();

    // IDukeEngine
    void onUpdate(DukeWidget&);
    void onEvent(const google::protobuf::serialize::MessageHolder&);

    // Communication
    void push(const google::protobuf::serialize::SharedHolder& holder);
    void pop(google::protobuf::serialize::SharedHolder& holder);

private:
    friend class RenderHelper;

    void consumeStateMessages();
    void updatePlayback();
    void setupClips();
    void render(DukeWidget&);
    void stateNotification();
    void consumeDebug(const ::duke::protocol::Debug&) const;
    void consumeScene(const ::duke::protocol::Scene&);
    void consumeTransport(const ::duke::protocol::Transport&, const ::google::protobuf::serialize::MessageHolder::Action);
    void consumePlaybackState(const ::duke::protocol::PlaybackState&);
    void consumeInfo(::duke::protocol::Info, const ::google::protobuf::serialize::MessageHolder::Action);
    void updateInfoPlaybackState(::duke::protocol::Info::PlaybackState &) const;
    void updateInfoCacheState(::duke::protocol::Info::CacheState &cache) const;
    void updateInfoImages(::google::protobuf::RepeatedPtrField<duke::protocol::Info::ImageInfo> &imageInfos) const;
    void updateInfoExtensions(::google::protobuf::RepeatedPtrField<std::string> &extensions) const;

    void updatePlaybackState();
    void applyTransport(const ::duke::protocol::Transport&);
    std::string dumpInfo(const ::duke::protocol::Debug::Content& info) const;

    enum PresentStatus { PRESENT_IMMEDIATE = 0, PRESENT_NEXT_BLANKING = 1, SKIP_NEXT_BLANKING = 2 };
    PresentStatus getPresentStatus();
    void waitForBlankingAndWarn(IRenderer& renderer, bool presented);
    void verticalBlanking(bool presented);

    ImageDecoderFactory &m_ImageDecoderFactory;
    ::duke::protocol::PlaybackState m_PlaybackState;
    ::duke::protocol::PlaylistHelper m_Playlist;
    ::duke::protocol::Cache m_CacheConfiguration;
    ::duke::protocol::Engine m_EngineStatus;
    playback::Playback m_Playback;
    AudioEngine m_AudioEngine;
    SmartCache m_Cache;
    FileBufferHolder m_FileBufferHolder;
    Timings m_VbiTimings;
    Timings m_FrameTimings;
    Durations m_Durations;
    uint32_t m_PreviousFrame;
    uint32_t m_CurrentFrame;
    uint32_t m_StoredFrame;
    uint32_t m_DisplayedFrameCount;
    bool m_bRequestTermination;
    bool m_bAutoNotifyOnFrameChange;
    bool m_bForceRefresh;

    IMessageIO &m_IO;
    MessageQueue m_RenderQueue;

    std::vector<duke::protocol::Clip> m_Clips;
    std::vector<ImageDescription> m_Images;
};

struct quit_exception : public std::runtime_error {
    const int returnValue;
    quit_exception(int returnValue) :
                    std::runtime_error("exiting"), returnValue(returnValue) {
    }
    quit_exception(int returnValue, const std::string &msg) :
                    std::runtime_error(msg), returnValue(returnValue) {
    }
};

duke::protocol::Renderer waitForRenderer(IMessageIO &);

#endif /* DUKE_ENGINE_H_ */
