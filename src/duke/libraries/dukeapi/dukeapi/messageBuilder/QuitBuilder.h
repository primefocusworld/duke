/*
 * QuitBuilder.h
 *
 *  Created on: 4 oct. 2011
 *      Author: Guillaume Chatelet, Nicolas Rondaud
 */

#ifndef QUITBUILDER_H_
#define QUITBUILDER_H_

#include "details/IBuilder.h"
#include <protocol.pb.h>
#include <cstdlib> // for EXIT_SUCCESS and EXIT_FAILURE

#define EXIT_RELAUNCH -1

namespace duke {
namespace protocol {

namespace gps = ::google::protobuf::serialize;

class QuitBuilder : public IBuilder {
public:
    static gps::MessageHolder failure();
    static gps::MessageHolder success();
    static gps::MessageHolder relaunch();
    virtual boost::shared_ptr<google::protobuf::Message> build(const std::string name = "");
private:
    static inline gps::MessageHolder closeConnection(int32_t exit_code) {
        gps::MessageHolder holder;
        holder.set_action(gps::MessageHolder::CLOSE_CONNECTION);
        holder.set_return_value(exit_code);
        return holder;
    }
};

} // protocol
} // duke

#endif /* QUITBUILDER_H_ */
