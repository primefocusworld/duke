/*
 * ResourceCache.cpp
 *
 *  Created on: Apr 17, 2012
 *      Author: Guillaume Chatelet
 */

#include "ResourceCache.h"

namespace resource {

SharedResourcePtr ResourceCache::get(EResourceType type, const std::string & name) {
    assert(type < UNDEF);
    ResourceMap &map = m_Map[type];
    const ResourceMap::iterator itr = map.find(name);
    if (itr == map.end())
        return SharedResourcePtr();
    return itr->second;
}

void ResourceCache::put(EResourceType type, const std::string & name, const SharedResourcePtr &pResource) {
    assert(type < UNDEF);
    ResourceMap &map = m_Map[type];
    map.insert(std::make_pair(name, pResource));
}

} /* namespace resource */
