#include "Enums.h"
#include "DisplayableImage.h"
#include "IRenderer.h"
#include "resource/ITextureBase.h"
#include "utils/ProtobufUtils.h"

#include <boost/bind.hpp>
#include <iostream>

using namespace std;
using namespace boost;

ImageDescription getImageDescriptionFrom(const ::duke::protocol::Texture& texture) {
    ImageDescription description;

    description.width = texture.width();
    description.height = texture.height();
    description.depth = texture.depth();
    if (texture.has_data()) {
        description.pImageData = texture.data().c_str();
        description.imageDataSize = texture.data().size();
    }
    description.format = Enums::Get(texture.format());
    return description;
}

DisplayableImage::DisplayableImage(IRenderer& renderer, const ::duke::protocol::Texture& texture) :
                m_Image(renderer.resourceCache, texture.name(), getImageDescriptionFrom(texture)) {
    m_pTexture = renderer.resourceCache.getOrBuild<ITextureBase>(texture.name(), //
                    bind(&IRenderer::createTexture, ref(renderer), getImageDescriptionFrom(texture), 0));
    updateTexture();
}

DisplayableImage::DisplayableImage(IRenderer& renderer, const string& name) :
                m_Image(renderer.resourceCache, name) {
    renderer.resourceCache.tryGet(name, m_pTexture);
}

const ImageDescription& DisplayableImage::getImageDescription() const {
    return m_Image.getImageDescription();
}

DisplayableImage::~DisplayableImage() {
}

void DisplayableImage::updateTexture() {
    IImageBase* pImage = m_Image.m_pImage.get();

    assert( pImage);
    assert( m_pTexture);
    m_pTexture->update(pImage->m_Description, &pImage->m_Pixels[0]);
}

ITextureBase* DisplayableImage::getTexture() const {
    return m_pTexture.get();
}

