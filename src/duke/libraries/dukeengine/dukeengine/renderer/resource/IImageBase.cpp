#include "IImageBase.h"
#include "../utils/PixelFormatUtils.h"

static inline size_t computeBufferSize(const ImageDescription& desc) {
    const int width = desc.width;
    const int height = desc.height;
    const int depth = std::max(desc.depth, 1);
    return width * height * depth * bytesPerPixel(desc.format);
}

IImageBase::IImageBase(const ImageDescription& imageDescription) :
                m_Description(imageDescription), m_Pixels(computeBufferSize(m_Description)) {
}

IImageBase::~IImageBase() {
}

const ImageDescription& IImageBase::getImageDescription() const {
    return m_Description;
}

