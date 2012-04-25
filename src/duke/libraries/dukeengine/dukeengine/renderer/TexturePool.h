/*
 * TexturePool.h
 *
 *  Created on: Apr 25, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef TEXTUREPOOL_H_
#define TEXTUREPOOL_H_

#include "resource/ResourceCache.h"
#include "resource/ITextureBase.h"
#include <dukeengine/renderer/Enums.h>
#include <dukeio/ImageDescription.h>

#include <boost/shared_ptr.hpp>

#include <vector>
#include <string>
#include <map>

class IRenderer;
struct VolatileTexture;

class TexturePool {
    struct Slot {
        std::string name;
        size_t refs;
        Slot():refs(0){}
        Slot(const std::string &name):name(name), refs(0){}
        void addRef(){++refs;}
        void release(){assert(refs>0);--refs;}
    };
    typedef boost::shared_ptr<Slot> SharedSlot;
    typedef std::vector<SharedSlot> SlotPtrList;
    typedef std::map<std::string, SlotPtrList> SlotMap;
    SlotMap m_Map;
    IRenderer& m_Renderer;
    friend struct VolatileTexture;
    static inline bool freeSlot(const TexturePool::SharedSlot &);
public:
    TexturePool(IRenderer& renderer) : m_Renderer(renderer){}
    VolatileTexture get(const ImageDescription& desc, int flags = 0);
};

struct VolatileTexture{
    VolatileTexture(){}
    VolatileTexture(const VolatileTexture& other);
    ~VolatileTexture();
    ITextureBase* getTexture() const {return pTexture.get();}
private:
    friend class TexturePool;
    TexturePool::SharedSlot pSlot;
    TexturePtr pTexture;
    VolatileTexture(TexturePool::SharedSlot pSlot, const TexturePtr &pTexture);
};

#endif /* TEXTUREPOOL_H_ */
