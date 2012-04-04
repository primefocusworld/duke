/*
 * RenderInterface.h
 *
 *  Created on: 4 avr. 2012
 *      Author: Guillaume Chatelet
 */

#ifndef RENDERINTERFACE_H_
#define RENDERINTERFACE_H_

#include <dukeio/ImageDescription.h>
#include <player.pb.h>
#include <vector>

namespace google { namespace protobuf { namespace serialize { class MessageHolder; } } }

struct Setup {
    size_t m_iFrame;
    std::vector<duke::protocol::Clip> m_Clips;
    std::vector<ImageDescription> m_Images;
};

struct IRendererHost {
    enum PresentStatus { PRESENT_IMMEDIATE = 0, PRESENT_NEXT_BLANKING = 1, SKIP_NEXT_BLANKING = 2 };

    virtual ~IRendererHost();
    virtual void renderStart() = 0;
    virtual void verticalBlanking(bool presented) = 0;
    virtual bool renderFinished(unsigned msToPresent) = 0;
    virtual PresentStatus getPresentStatus() = 0;
    virtual void pushEvent(const google::protobuf::serialize::MessageHolder&) = 0;
    virtual const google::protobuf::serialize::MessageHolder * popEvent() = 0;

    Setup m_Setup;
};

void createWindowAndLoop(duke::protocol::Renderer, IRendererHost&);

#endif /* RENDERINTERFACE_H_ */
