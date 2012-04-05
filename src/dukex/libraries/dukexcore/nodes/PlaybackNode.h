#ifndef PLAYBACKNODE_H
#define PLAYBACKNODE_H

#include <dukexcore/dkxINode.h>
#include "player.pb.h"

class PlaybackNode : public INode {

public:
    typedef boost::shared_ptr<PlaybackNode> ptr;
    PlaybackNode() :
        INode("fr.mikrosimage.dukex.playback") {
    }

public:
    void registerFramerate(float framerate) {
        SessionDescriptor & descriptor = session()->descriptor();
        descriptor.setFramerate(framerate);
    }

    void registerPlaybackMode(duke::protocol::PlaybackState::PlaybackMode mode) {
        SessionDescriptor & descriptor = session()->descriptor();
        descriptor.setPlaybackMode(mode);
    }

    bool setFramerate(float framerate) {
        try {
            MessageQueue queue;
            duke::protocol::PlaybackState playback;
            playback.set_frameratenumerator(framerate);
            push(queue, playback);

            registerFramerate(framerate);
            session()->sendMsg(queue);
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }

    bool setPlaybackMode(duke::protocol::PlaybackState::PlaybackMode mode) {
        try {
            MessageQueue queue;
            duke::protocol::PlaybackState playback;
            playback.set_playbackmode(mode);
            push(queue, playback);

            registerPlaybackMode(mode);
            session()->sendMsg(queue);
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }


};

#endif // PLAYBACKNODE_H
