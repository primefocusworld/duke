/*
 * TransportBuilder.cpp
 *
 *  Created on: 12 juin 2012
 *      Author: Nicolas Rondaud
 */

#include "TransportBuilder.h"
#include <boost/make_shared.hpp>
#include <stdexcept>

using namespace duke::protocol;

Transport TransportBuilder::play() {
    return MAKE(Transport::PLAY);
}

Transport TransportBuilder::stop() {
    return MAKE(Transport::STOP);
}

Transport TransportBuilder::gotoFrame(size_t _f) {
    return MAKE(Transport::CUE, _f);
}

Transport TransportBuilder::previousFrame() {
    return MAKE(Transport::CUE, -1, true);
}

Transport TransportBuilder::nextFrame() {
    return MAKE(Transport::CUE, 1, true);
}

Transport TransportBuilder::firstFrame() {
    return MAKE(Transport::CUE_FIRST);
}

Transport TransportBuilder::lastFrame() {
    return MAKE(Transport::CUE_LAST);
}

Transport TransportBuilder::previousShot() {
    return MAKE(Transport::CUE, -1, true, true);
}

Transport TransportBuilder::nextShot() {
    return MAKE(Transport::CUE, 1, true, true);
}

boost::shared_ptr<google::protobuf::Message> TransportBuilder::build(const std::string name) {
    return boost::shared_ptr<Cache>();
}

// private
Transport TransportBuilder::MAKE(const Transport::TransportType type, const int value, const bool cueRelative, const bool cueClip) {
    Transport transport;
    transport.set_type(type);
    transport.set_autonotifyonframechange(true);
    if (type == Transport::CUE) {
        if (!cueRelative && value < 0)
            throw std::runtime_error("can't cue to a negative frame");
        Transport_Cue *cue = transport.mutable_cue();
        cue->set_cueclip(cueClip);
        cue->set_cuerelative(cueRelative);
        cue->set_value(value);
    }
    return transport;
}
