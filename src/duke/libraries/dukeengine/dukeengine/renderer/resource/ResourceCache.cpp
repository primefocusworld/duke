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
    ResourceMap::iterator itr = map.find(name);
    if (itr != map.end())
        itr->second = pResource;
    else
        map.insert(std::make_pair(name, pResource));
}

#ifdef DEBUG
static const char* const toString(EResourceType type) {
    switch (type) {
        case MESH:
            return "MESH";
        case IMAGE:
            return "IMAGE";
        case SHADER:
            return "SHADER";
        case TEXTURE:
            return "TEXTURE";
        case PROTOBUF:
            return "PROTOBUF";
        case UNDEF:
        default:
            return "UNDEF";
    }
}
#endif

ResourceCache::~ResourceCache() {
#ifdef DEBUG
    std::cout << "[ResourceCache] dumping cached resources" << std::endl;
    for (int i = 0; i < UNDEF; ++i) {
        std::cout << toString(EResourceType(i)) << " :" << std::endl;
        ResourceMap &map = m_Map[i];
        ResourceMap::const_iterator itr(map.begin()), end(map.end());
        for (; itr != end; ++itr)
            std::cout << "- '" << itr->first << "'" << std::endl;
    }
#endif
}
} /* namespace resource */
