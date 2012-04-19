#ifndef TEXTUREBASE_H_
#define TEXTUREBASE_H_

#include "Enums.h"
#include "math/Vector2.h"
#include "math/Rect.h"
#include "ResourceCache.h"
#include "Image.h"

struct ImageSpec;

class ITextureBase : public ::resource::IResource
{
public:
	virtual ~ITextureBase();

	bool         isMipMap() const;
	bool         isAutoMipMap() const;
	bool         isRenderTarget() const;
	int          getWidth() const;
	int          getHeight() const;
	int          getDepth() const;
	TPixelFormat getFormat() const;

protected:
	friend class VolatileTexture;
	friend class DisplayableImage;
	ITextureBase( const ImageDescription& description, const TPixelFormat texturePixelFormat, long usageFlag );
	virtual void update( const ImageDescription& description, const unsigned char* pData ) = 0;

	const ImageDescription m_Description;
	const TPixelFormat m_TexturePixelFormat;
	const long m_iUsageFlag;
};

#endif /* TEXTUREBASE_H_ */
