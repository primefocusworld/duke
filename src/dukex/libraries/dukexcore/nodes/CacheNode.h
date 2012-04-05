#ifndef CACHENODE_H
#define CACHENODE_H

#include <dukexcore/dkxINode.h>
#include "player.pb.h"

class CacheNode : public INode {

public:
    typedef boost::shared_ptr<CacheNode> ptr;
    CacheNode() :
        INode("fr.mikrosimage.dukex.cache") {
    }

public:
    bool setSize(size_t size) {
        try {
            MessageQueue queue;
            duke::protocol::Cache cache;
            cache.set_size(size);
            push(queue, cache);
            session()->sendMsg(queue);
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }

    bool setThreadingSize(size_t size) {
        try {
            MessageQueue queue;
            duke::protocol::Cache cache;
            cache.set_threading(size);
            push(queue, cache);
            session()->sendMsg(queue);
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }

    bool disable() {
        return setSize(0);
    }

};

#endif // CACHENODE_H
