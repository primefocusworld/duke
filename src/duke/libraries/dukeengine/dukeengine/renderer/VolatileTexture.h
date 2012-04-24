#ifndef VOLATILETEXTURE_H_
#define VOLATILETEXTURE_H_

#include "TexturePool.h"
#include <dukeio/ImageDescription.h>
#include <string>

class IRenderer;

class VolatileTexture {
public:
    VolatileTexture(){}
    VolatileTexture(IRenderer& renderer, const ImageDescription& spec, const unsigned flags = 0);
    virtual ~VolatileTexture(){}

    virtual ITextureBase* getTexture() const;
    virtual const ImageDescription& getImageDescription() const;

private:
    ImageDescription m_Description;
    TexturePool::ScopedTexturePtr m_pTexture;
};

#endif /* VOLATILETEXTURE_H_ */
