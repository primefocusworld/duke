#include "IImageBase.h"
#include "../utils/PixelFormatUtils.h"
#include <string.h>

static inline size_t computeBufferSize(const ImageDescription& desc) {
    const int width = desc.width;
    const int height = desc.height;
    const int depth = std::max(desc.depth, 1);
    return width * height * depth * bytesPerPixel(desc.format);
}

IImageBase::IImageBase(const ImageDescription& imageDescription) :
                m_Description(imageDescription), m_Pixels(computeBufferSize(m_Description)) {
    if (imageDescription.imageDataSize != 0) {
        assert( m_Pixels.size() >= imageDescription.imageDataSize);
        memcpy(m_Pixels.data(), imageDescription.pImageData, imageDescription.imageDataSize);
    }
}

IImageBase::~IImageBase() {
}

const ImageDescription& IImageBase::getImageDescription() const {
    return m_Description;
}

const IImageBase::Pixels& IImageBase::pixels() const{
    return m_Pixels;
}
