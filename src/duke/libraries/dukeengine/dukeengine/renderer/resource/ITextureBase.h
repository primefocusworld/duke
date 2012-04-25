#ifndef TEXTUREBASE_H_
#define TEXTUREBASE_H_

#include "IResource.h"
#include <dukeengine/renderer/Enums.h>
#include <dukeio/ImageDescription.h>

struct ImageSpec;

class ITextureBase : public IResource {
public:
    virtual ~ITextureBase();

    inline bool isMipMap() const { return (m_iUsageFlag & TEX_MIPMAP) > 0; }
    inline bool isAutoMipMap() const { return (m_iUsageFlag & TEX_AUTOMIPMAP) > 0; }
    inline bool isRenderTarget() const {return (m_iUsageFlag & TEX_RENTERTARGET) > 0;}
    inline int getWidth() const {return m_Description.width;}
    inline int getHeight() const {return m_Description.height;}
    inline int getDepth() const {return m_Description.depth;}
    inline TPixelFormat getFormat() const {return m_TexturePixelFormat;}
    inline const ImageDescription& getImageDescription() const {return m_Description;}

    virtual void update(const unsigned char* pData) = 0;


protected:
    friend class VolatileTexture;
    friend class DisplayableImage;
    ITextureBase(const ImageDescription& description, const TPixelFormat texturePixelFormat, long usageFlag);

    const ImageDescription m_Description;
    const TPixelFormat m_TexturePixelFormat;
    const long m_iUsageFlag;
};

#include <boost/shared_ptr.hpp>
typedef ::boost::shared_ptr<ITextureBase> TexturePtr;

#endif /* TEXTUREBASE_H_ */
