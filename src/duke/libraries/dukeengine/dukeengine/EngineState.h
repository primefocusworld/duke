/*
 * EngineState.h
 *
 *  Created on: May 2, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef ENGINESTATE_H_
#define ENGINESTATE_H_

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

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

// fwd declaration
namespace duke {namespace protocol {struct PlaylistHelper;}}
namespace google {namespace protobuf {class Message;}}

struct EngineState : public boost::noncopyable {
    EngineState(ImageDecoderFactoryImpl& imageDecoderFactory, const duke::protocol::Cache& cacheConfiguration);

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
};

struct quit_exception : public std::runtime_error{
    const int returnValue;
    quit_exception(int returnValue) : std::runtime_error("exiting"), returnValue(returnValue){}
};

duke::protocol::Renderer waitForRenderer(EngineState& engineState, IMessageIO &);

#endif /* ENGINESTATE_H_ */
