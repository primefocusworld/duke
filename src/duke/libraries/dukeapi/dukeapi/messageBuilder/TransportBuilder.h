/*
 * TransportBuilder.h
 *
 *  Created on: 12 juin 2012
 *      Author: Nicolas Rondaud
 */

#ifndef TRANSPORTBUILDER_H
#define TRANSPORTBUILDER_H

#include "details/IBuilder.h"
#include "player.pb.h"

namespace duke {
namespace protocol {

class TransportBuilder : public IBuilder {
public:
    static Transport play();
    static Transport stop();
    static Transport gotoFrame(size_t _f);
    static Transport previousFrame();
    static Transport nextFrame();
    static Transport firstFrame();
    static Transport lastFrame();
    static Transport previousShot();
    static Transport nextShot();
    virtual boost::shared_ptr<google::protobuf::Message> build(const std::string name = "");
private:
    static Transport MAKE(const Transport::TransportType type //
                    , const int value = -1 //
                    , const bool cueRelative = false //
                    , const bool cueClip = false);
};

} // namespace protocol
} // namespace duke

#endif // TRANSPORTBUILDER_H
