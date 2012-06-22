/*
 * PlaybackBuilder.h
 *
 *  Created on: 12 juin 2012
 *      Author: Nicolas Rondaud
 */

#ifndef PLAYBACKBUILDER_H
#define PLAYBACKBUILDER_H

#include "details/IBuilder.h"
#include "player.pb.h"

namespace duke {
namespace protocol {

class CacheBuilder : public IBuilder {
public:
    static PlaybackState setFramerate(float framerate);
    static PlaybackState setPlaybackMode(duke::protocol::PlaybackState::PlaybackMode mode);
    virtual boost::shared_ptr<google::protobuf::Message> build(const std::string name = "");
};

}  // namespace protocol
}  // namespace duke

#endif // PLAYBACKBUILDER_H
