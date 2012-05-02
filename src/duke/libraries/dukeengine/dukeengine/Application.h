#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "RenderInterface.h"
#include "time_statistics/Durations.h"
#include "audio/AudioEngine.h"
#include "playback/Playback.h"
#include "playback/Timings.h"
#include "image/SmartCache.h"
#include "image/FileBufferHolder.h"
#include "host/io/ImageDecoderFactoryImpl.h"

#include <protocol.pb.h>

#include <dukeapi/sequence/PlaylistHelper.h>
#include <dukeapi/MessageQueue.h>


#include <boost/shared_ptr.hpp>

// fwd declaration
class Renderer;
namespace duke {namespace protocol {struct PlaylistHelper;}}
namespace google {namespace protobuf {class Message;}}

class Application : public IRendererHost {
public:
    Application(ImageDecoderFactoryImpl& imageDecoderFactory, IMessageIO &IO, int &returnCode, const duke::protocol::Cache& cacheConfigurationThreads);

    void renderStart();
    void verticalBlanking(bool presented);
    bool renderFinished(unsigned msToPresent);
    PresentStatus getPresentStatus();
    void pushEvent(const google::protobuf::serialize::MessageHolder&);
    const google::protobuf::serialize::MessageHolder * popEvent();

private:
    void consumeMessages();
    void consumeDebug(const ::duke::protocol::Debug&) const;
    void consumeScene(const ::duke::protocol::Scene&);
    void consumePlaybackState(const ::duke::protocol::PlaybackState&);
    void consumeTransport(const ::duke::protocol::Transport&, const ::google::protobuf::serialize::MessageHolder::Action);
    void consumeInfo(::duke::protocol::Info, const ::google::protobuf::serialize::MessageHolder::Action);
    void consumeCache(const ::duke::protocol::Cache&, const ::google::protobuf::serialize::MessageHolder::Action);
    void updatePlaybackState();
    void updateInfoPlaybackState(::duke::protocol::Info::PlaybackState &) const ;
    void updateInfoCacheState(::duke::protocol::Info::CacheState &cache) const ;
    void updateInfoImages(::google::protobuf::RepeatedPtrField<duke::protocol::Info::ImageInfo> &imageInfos) const ;
    void updateInfoExtensions(::google::protobuf::RepeatedPtrField<std::string> &extensions) const ;

    void applyTransport(const ::duke::protocol::Transport&);
    void consumeUntilRenderOrQuit();
    bool handleQuitMessage(const ::google::protobuf::serialize::SharedHolder&);
    std::string dumpInfo(const ::duke::protocol::Debug::Content& debug) const;

    void updatePlayback();

private:
    // order of variables are very important because of multi threading issues
    IMessageIO &m_IO;
    MessageQueue m_RendererMessages;
    ::google::protobuf::serialize::SharedHolder m_RendererMessageHolder;
    ImageDecoderFactoryImpl &m_ImageDecoderFactory;
    ::duke::protocol::PlaybackState m_PlaybackState;
    ::duke::protocol::PlaylistHelper m_Playlist;
    ::duke::protocol::Cache m_CacheConfiguration;
    playback::Playback m_Playback;
    AudioEngine m_AudioEngine;
    SmartCache m_Cache;
    FileBufferHolder m_FileBufferHolder;
    Timings m_VbiTimings;
    Timings m_FrameTimings;
    Durations m_Durations;
    uint32_t m_PreviousFrame;
    uint32_t m_StoredFrame;
    bool m_bRequestTermination;
    bool m_bAutoNotifyOnFrameChange;
    bool m_bForceRefresh;
    int &m_iReturnCode;
};

#endif /* APPLICATION_H_ */
