/*
 * PlaybackBuilder.cpp
 *
 *  Created on: 12 juin 2012
 *      Author: Nicolas Rondaud
 */

#include "PlaybackBuilder.h"
#include <boost/make_shared.hpp>

namespace duke {
namespace protocol {

PlaybackState setFramerate(float framerate) {
    duke::protocol::PlaybackState playback;
    playback.set_frameratenumerator(framerate);
    return playback;
}

PlaybackState setPlaybackMode(duke::protocol::PlaybackState::PlaybackMode mode) {
    duke::protocol::PlaybackState playback;
    playback.set_playbackmode(mode);
    return playback;
}

boost::shared_ptr<google::protobuf::Message> CacheBuilder::build(const std::string name) {
    return boost::shared_ptr<PlaybackState>();
}

} // namespace protocol
} // namespace duke
