/*
 * EngineState.cpp
 *
 *  Created on: May 2, 2012
 *      Author: Guillaume Chatelet
 */

#include "EngineState.h"

EngineState::EngineState(ImageDecoderFactoryImpl& imageDecoderFactory, const duke::protocol::Cache& cacheConfiguration) :
                m_ImageDecoderFactory(imageDecoderFactory), //
                m_CacheConfiguration(cacheConfiguration), //
                m_AudioEngine(), //
                m_Cache(m_CacheConfiguration, m_ImageDecoderFactory), //
                m_FileBufferHolder(), //
                m_VbiTimings(VBI, 120), //
                m_FrameTimings(FRAME, 10), //
                m_PreviousFrame(-1), //
                m_StoredFrame(-1), //
                m_bRequestTermination(false), //
                m_bAutoNotifyOnFrameChange(false), //
                m_bForceRefresh(true) {
}

using namespace ::duke::protocol;
using namespace ::google::protobuf::serialize;
using namespace ::google::protobuf;

Renderer waitForRenderer(EngineState& engineState, IMessageIO &io) {
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
