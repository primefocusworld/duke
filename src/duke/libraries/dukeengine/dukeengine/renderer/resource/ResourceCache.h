/*
 * ResourceCache.h
 *
 *  Created on: Apr 17, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef RESOURCECACHE_H_
#define RESOURCECACHE_H_

#include "IResource.h"
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

class IMeshBase;
class IImageBase;
class IShaderBase;
class ITextureBase;
class ProtoBufResource;

namespace resource {

enum EResourceType {
    MESH = 0, IMAGE = 1, SHADER = 2, TEXTURE = 3, PROTOBUF = 4, UNDEF = 5
};

typedef boost::shared_ptr<IResource> SharedResourcePtr;

struct ResourceCache {
    SharedResourcePtr get(EResourceType type, const std::string & name);
    void put(EResourceType type, const std::string & name, const SharedResourcePtr &pResource);
private:
    typedef boost::unordered::unordered_map<std::string, SharedResourcePtr> ResourceMap;
    ResourceMap m_Map[UNDEF];
};

template<typename T>
inline EResourceType getType();

template<typename T>
inline boost::shared_ptr<T> & tryGet(ResourceCache& cache, const std::string & name, boost::shared_ptr<T> &result) {
    result = boost::shared_dynamic_cast<T>(cache.get(getType<T>(), name));
    return result;
}

template<typename T>
T& get(ResourceCache& cache, const std::string & name) {
    SharedResourcePtr ptr = cache.get(getType<T>(), name);
    if (ptr == NULL)
        throw std::logic_error(std::string("no resource named : ") + name);
    return *boost::shared_dynamic_cast<T>(ptr);
}

template<typename T>
inline void put(ResourceCache& cache, const std::string & name, const boost::shared_ptr<T> &pResource) {
    cache.put(getType<T>(), name, pResource);
}

template<>
inline EResourceType getType<IMeshBase>() {
    return MESH;
}
template<>
inline EResourceType getType<IImageBase>() {
    return IMAGE;
}
template<>
inline EResourceType getType<IShaderBase>() {
    return SHADER;
}
template<>
inline EResourceType getType<ITextureBase>() {
    return TEXTURE;
}
template<>
inline EResourceType getType<ProtoBufResource>() {
    return PROTOBUF;
}

} /* namespace resource */

#endif /* RESOURCECACHE_H_ */
