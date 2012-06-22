/*
 * InfoBuilder.cpp
 *
 *  Created on: 12 juin 2012
 *      Author: Nicolas Rondaud
 */

#include "InfoBuilder.h"
#include <boost/make_shared.hpp>

namespace duke {
namespace protocol {

Info InfoBuilder::askForCurrentImageInfo() {
    Info info;
    info.set_content(Info_Content_IMAGEINFO);
    return info;
}

Info InfoBuilder::askForCurrentCacheState() {
    Info info;
    info.set_content(Info_Content_CACHESTATE);
    return info;
}

Info InfoBuilder::askForCurrentPlaybackState() {
    Info info;
    info.set_content(Info_Content_PLAYBACKSTATE);
    return info;
}

boost::shared_ptr<google::protobuf::Message> InfoBuilder::build(const std::string name) {
    return boost::shared_ptr<Info>();
}

} // namespace protocol
} // namespace duke
