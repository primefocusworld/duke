/*
 * EngineState.cpp
 *
 *  Created on: May 2, 2012
 *      Author: Guillaume Chatelet
 */

#include "DukeEngine.h"

#include "DukeWidget.h"
#include <dukeapi/ProtobufSerialize.h>

#include <iterator>

using namespace ::duke::protocol;
using namespace ::google::protobuf;
using namespace ::google::protobuf::serialize;

IDukeEngine::~IDukeEngine() {
}

DukeEngine::DukeEngine(ImageDecoderFactory& imageDecoderFactory, const duke::protocol::Cache& cacheConfiguration, IMessageIO &io) :
                m_ImageDecoderFactory(imageDecoderFactory), //
                m_CacheConfiguration(cacheConfiguration), //
                m_AudioEngine(), //
                m_Cache(m_CacheConfiguration, m_ImageDecoderFactory), //
                m_FileBufferHolder(), //
                m_VbiTimings(VBI, 120), //
                m_FrameTimings(FRAME, 10), //
                m_PreviousFrame(-1), //
                m_StoredFrame(-1), //
                m_DisplayedFrameCount(0), //
                m_bRequestTermination(false), //
                m_bAutoNotifyOnFrameChange(false), //
                m_bForceRefresh(true), //
                m_IO(io) {
    m_EngineStatus.set_action(Engine::RENDER_START);
}

DukeEngine::~DukeEngine() {
}

void DukeEngine::push(const google::protobuf::serialize::SharedHolder& holder) {
    m_IO.push(holder);
}

void DukeEngine::pop(google::protobuf::serialize::SharedHolder& holder) {
    m_IO.waitPop(holder);
}

/////////////////
// IDukeEngine //
/////////////////

void DukeEngine::onEvent(const google::protobuf::serialize::MessageHolder& holder) {
    push(serialize::make_shared(holder));
}

void DukeEngine::onUpdate(DukeWidget& widget) {
    consumeStateMessages();
    updatePlayback();
    setupClips();
    render(widget);
    stateNotification();
}

void DukeEngine::stateNotification() {
    const uint32_t newFrame = m_Playback.frame();
    if (m_PreviousFrame != newFrame && m_bAutoNotifyOnFrameChange) {
        Transport transport;
        transport.set_type(Transport::CUE);
        Transport_Cue *cue = transport.mutable_cue();
        cue->set_value(newFrame);
        push(packAndShare(transport));
    }
    //        if(m_PlaybackState.isLastFrame() || newFrame < m_PreviousFrame){
    //            m_AudioEngine.rewind();
    //        }
    m_PreviousFrame = newFrame;
}

void DukeEngine::updatePlayback() {
    m_Playback.adjustCurrentFrame();
    if (m_Playback.playing())
        m_AudioEngine.checksync(m_Playback.playlistTime());
}

void DukeEngine::setupClips() {
    m_CurrentFrame = m_Playback.frame();

    if (m_bForceRefresh || m_PreviousFrame != m_CurrentFrame) {
        const int32_t speed = m_Playback.getSpeed();
        const EPlaybackState state = speed == 0 ? BALANCE : (speed > 0 ? FORWARD : REVERSE);
        m_Cache.seek(m_CurrentFrame, state);
        m_FileBufferHolder.update(m_CurrentFrame, m_Cache, m_Playlist);
        m_bForceRefresh = false;
    }

    typedef std::vector<ImageHolder> Images;
    const Images& images = m_FileBufferHolder.getImages();

    m_Images.clear();
    for (Images::const_iterator itr = images.begin(); itr != images.end(); ++itr)
        m_Images.push_back(itr->getImageDescription());
    m_Playlist.clipsAt(m_CurrentFrame, m_Clips);
}

#include <google/protobuf/descriptor.h>

Renderer waitForRenderer(IMessageIO &io) {
    SharedHolder pHolder;
    io.waitPop(pHolder);

    if (!pHolder)
        throw quit_exception(EXIT_FAILURE);

    if (pHolder->action() == MessageHolder::CLOSE_CONNECTION)
        throw quit_exception(pHolder->return_value());

    const MessageHolder &holder = *pHolder;
    const Descriptor* descriptor = descriptorFor(holder);

    if (!isType<Renderer>(descriptor))
        throw quit_exception(EXIT_FAILURE, "First message must be either Renderer or Quit, ignoring message of type " + descriptor->name());

    return unpackTo<Renderer>(holder);
}
