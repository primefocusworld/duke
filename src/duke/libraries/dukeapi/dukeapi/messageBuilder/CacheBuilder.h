/*
 * CacheBuilder.h
 *
 *  Created on: 12 juin 2012
 *      Author: Nicolas Rondaud
 */

#ifndef CACHEBUILDER_H
#define CACHEBUILDER_H

#include "details/IBuilder.h"
#include "player.pb.h"

namespace duke {
namespace protocol {

class CacheBuilder : public IBuilder {
public:
    static Cache setSize(int32_t size);
    static Cache setThreadingSize(int32_t size);
    static Cache disable();
    virtual boost::shared_ptr<google::protobuf::Message> build(const std::string name = "");
};

} // namespace protocol
} // namespace duke

#endif // CACHEBUILDER_H
