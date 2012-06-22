/*
 * CacheBuilder.cpp
 *
 *  Created on: 12 juin 2012
 *      Author: Nicolas Rondaud
 */

#include "CacheBuilder.h"
#include <boost/make_shared.hpp>

namespace duke {
namespace protocol {

Cache CacheBuilder::setSize(int32_t size) {
    Cache cache;
    cache.set_size(size);
    return cache;
}

Cache CacheBuilder::setThreadingSize(int32_t size) {
    Cache cache;
    cache.set_threading(size);
    return cache;
}

Cache CacheBuilder::disable() {
    return setSize(0);
}

boost::shared_ptr<google::protobuf::Message> CacheBuilder::build(const std::string name) {
    return boost::shared_ptr<Cache>();
}

} // namespace protocol
} // namespace duke
