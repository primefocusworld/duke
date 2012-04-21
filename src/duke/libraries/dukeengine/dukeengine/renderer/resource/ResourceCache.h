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
#include <boost/function.hpp>

class IMeshBase;
class IImageBase;
class IShaderBase;
class ITextureBase;
class ProtoBufResource;

namespace resource {

enum EResourceType {
    MESH = 0, IMAGE = 1, SHADER = 2, TEXTURE = 3, PROTOBUF = 4, UNDEF = 5
};

template<typename T>
inline EResourceType TYPE();

typedef boost::shared_ptr<IResource> SharedResourcePtr;

struct ResourceCache {
    SharedResourcePtr get(EResourceType type, const std::string & name);

    void put(EResourceType type, const std::string & name, const SharedResourcePtr &pResource);

    template<typename T>
    inline boost::shared_ptr<T> & tryGet(const std::string & name, boost::shared_ptr<T> &result) {
        result = boost::shared_dynamic_cast<T>(get(TYPE<T>(), name));
        return result;
    }

    template<typename T>
    T& get(const std::string & name) {
        SharedResourcePtr ptr = get(TYPE<T>(), name);
        if (ptr == NULL)
            throw std::logic_error(std::string("no resource named : ") + name);
        return *boost::shared_dynamic_cast<T>(ptr);
    }

    template<typename T>
    inline void put(const std::string & name, const boost::shared_ptr<T> &pResource) {
        if (!name.empty())
            put(TYPE<T>(), name, pResource);
    }

    template<typename T>
    boost::shared_ptr<T> getOrBuild(const std::string & name, boost::function<T* ()> builder) {
        boost::shared_ptr<T> ptr = boost::shared_dynamic_cast<T>(get(TYPE<T>(), name));
        if (ptr != NULL)
            return ptr;
        ptr.reset(builder());
        if (ptr == NULL)
            throw std::logic_error("builder function returned a NULL T pointer");
        put<T>(name, ptr);
        return ptr;
    }
private:
    typedef boost::unordered::unordered_map<std::string, SharedResourcePtr> ResourceMap;
    ResourceMap m_Map[UNDEF];
};

template<>
inline EResourceType TYPE<IMeshBase>() {
    return MESH;
}
template<>
inline EResourceType TYPE<IImageBase>() {
    return IMAGE;
}
template<>
inline EResourceType TYPE<IShaderBase>() {
    return SHADER;
}
template<>
inline EResourceType TYPE<ITextureBase>() {
    return TEXTURE;
}
template<>
inline EResourceType TYPE<ProtoBufResource>() {
    return PROTOBUF;
}

} /* namespace resource */

#endif /* RESOURCECACHE_H_ */
