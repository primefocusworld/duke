#include "VolatileTexture.h"
#include "IRenderer.h"
#include "ITextureBase.h"
#include <dukeio/ImageDescription.h>
#include <iostream>
#include <cassert>

VolatileTexture::VolatileTexture(IRenderer& renderer, const ImageDescription& description, const unsigned flags) :
    m_Description(description) {
    const PoolRequest request(description.format, flags && TEX_MIPMAP > 0, description.width, description.height);

    m_pTexture = renderer.texturePool.get(request);
    if (!m_pTexture) {
        const resource::SharedResourcePtr pTexture(renderer.createTexture(description, flags));
        assert( pTexture );
        m_pTexture = renderer.texturePool.putAndGet(request, pTexture);
    }
    assert( getTexture() );
    if (!getTexture()->isRenderTarget()) // image not to be render to ? we must update it
        getTexture()->update(description, reinterpret_cast<const unsigned char*> (description.pImageData));
}

VolatileTexture::~VolatileTexture() {
}

ITextureBase* VolatileTexture::getTexture() const {
    return m_pTexture ? m_pTexture->getTexture<ITextureBase> () : NULL;
}

const ImageDescription& VolatileTexture::getImageDescription() const {
    return m_Description;
}

