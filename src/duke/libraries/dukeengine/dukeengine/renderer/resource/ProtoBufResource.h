#ifndef PROTOBUFRESOURCE_H_
#define PROTOBUFRESOURCE_H_

#include "IResource.h"
#include "ResourceCache.h"
#include <google/protobuf/message.h>
#include <boost/shared_ptr.hpp>
#include <cassert>

class ProtoBufResource : public IResource {
public:
    ProtoBufResource(const boost::shared_ptr<google::protobuf::Message> & shared);
    ProtoBufResource(const ::google::protobuf::Message& message);
    virtual ~ProtoBufResource();

    template<class MESSAGE_TYPE>
    const inline MESSAGE_TYPE* get() const {
        assert( m_pMessage);
        return dynamic_cast<MESSAGE_TYPE*>(m_pMessage.get());
    }

    template<class MESSAGE_TYPE>
    const inline MESSAGE_TYPE& getRef() const {
        return *get<MESSAGE_TYPE>();
    }

private:
    const boost::shared_ptr<google::protobuf::Message> m_pMessage;
};

namespace resource {

template<typename T>
inline const T& getPB(ResourceCache& cache, const std::string & name) {
    return cache.get<ProtoBufResource>(name).getRef<T>();
}

} // namespace resource

typedef boost::shared_ptr<ProtoBufResource> TResourcePtr;

#endif /* PROTOBUFRESOURCE_H_ */
