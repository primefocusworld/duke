/*
 * QuitBuilder.cpp
 *
 *  Created on: 12 juin 2012
 *      Author: Nicolas Rondaud
 */

#include "QuitBuilder.h"
#include <boost/make_shared.hpp>

using namespace duke::protocol;
using namespace ::google::protobuf::serialize;

MessageHolder QuitBuilder::failure() {
    return closeConnection(EXIT_FAILURE);
}

MessageHolder QuitBuilder::success() {
    return closeConnection(EXIT_SUCCESS);
}

MessageHolder QuitBuilder::relaunch() {
    return closeConnection(EXIT_RELAUNCH);
}

boost::shared_ptr<google::protobuf::Message> QuitBuilder::build(const std::string name) {
    return boost::make_shared < MessageHolder > ();
}
