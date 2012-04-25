/*
 * TexturePool.cpp
 *
 *  Created on: Apr 25, 2012
 *      Author: Guillaume Chatelet
 */

#include "TexturePool.h"
#include "utils/PixelFormatUtils.h"
#include "IRenderer.h"
#include <sstream>

using namespace std;

static string makeKey(const TPixelFormat& format, bool mipmap, int width, int height, int depth) {
    ostringstream ss;
    ss << toString(format) << '_' << width << '.' << height << '.' << depth << '(' << mipmap << ')';
    return ss.str();
}

inline bool TexturePool::freeSlot(const SharedSlot &pSlot) {
    return pSlot->refs == 0;
}

VolatileTexture TexturePool::get(const ImageDescription& desc, int flags) {
    string key = makeKey(desc.format, flags && TEX_MIPMAP > 0, desc.width, desc.height, desc.depth);
    SlotPtrList &ptrList = m_Map[key];
    SlotPtrList::iterator slotItr = std::find_if(ptrList.begin(), ptrList.end(), &TexturePool::freeSlot);
    TexturePtr pTexture;
    SharedSlot pSlot;
    if (slotItr == ptrList.end()) {
        ostringstream ss;
        ss << key << '_' << ptrList.size();
        const string name = ss.str();
        pTexture.reset(m_Renderer.createTexture(desc, flags));
        m_Renderer.resourceCache.put(name, pTexture);
        pSlot.reset(new Slot(name));
        ptrList.push_back(pSlot);
    } else {
        pSlot = *slotItr;
        m_Renderer.resourceCache.tryGet<ITextureBase>(pSlot->name, pTexture);
    }
    assert(pSlot);
    assert(pTexture);
    if (!pTexture->isRenderTarget()) // image not to be render to ? we must update it
        pTexture->update(reinterpret_cast<const unsigned char*>(desc.pImageData));
    return VolatileTexture(pSlot, pTexture);
}

VolatileTexture::VolatileTexture(const VolatileTexture& other) :
                pSlot(other.pSlot), pTexture(other.pTexture) {
    pSlot->addRef();
}

VolatileTexture::~VolatileTexture() {
    if (pSlot)
        pSlot->release();
}

VolatileTexture::VolatileTexture(TexturePool::SharedSlot pSlot, const TexturePtr &pTexture) :
                pSlot(pSlot), pTexture(pTexture) {
    assert(pTexture);
    assert(pSlot);
    pSlot->addRef();
}
