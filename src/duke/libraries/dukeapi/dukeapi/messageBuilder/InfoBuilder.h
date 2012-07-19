/*
 * InfoBuilder.h
 *
 *  Created on: 12 juin 2012
 *      Author: Nicolas Rondaud
 */

#ifndef INFOBUILDER_H
#define INFOBUILDER_H

#include "details/IBuilder.h"
#include <player.pb.h>

namespace duke {
namespace protocol {

class InfoBuilder : public IBuilder {

public:
    static Info askForCurrentImageInfo();
    static Info askForCurrentCacheState();
    static Info askForCurrentPlaybackState();
    virtual boost::shared_ptr<google::protobuf::Message> build(const std::string name = "");
};

} // namespace protocol
} // namespace duke

#endif // INFOBUILDER_H
