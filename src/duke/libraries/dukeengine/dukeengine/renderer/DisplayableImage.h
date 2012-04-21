#ifndef DISPLAYABLEIMAGE_H_
#define DISPLAYABLEIMAGE_H_

#include "Image.h"
#include "ITextureProvider.h"
#include "ITextureBase.h"

class DisplayableImage : public ITextureProvider
{
public:
    DisplayableImage( IRenderer& renderer, const ::duke::protocol::Texture& texture );
	DisplayableImage( IRenderer& renderer, const std::string& name );
	virtual ~DisplayableImage();

	void updateTexture();

	virtual const ImageDescription& getImageDescription() const;
	virtual ITextureBase*           getTexture() const;

private:
	Image m_Image;
	::boost::shared_ptr<ITextureBase> m_pTexture;
};

#endif /* DISPLAYABLEIMAGE_H_ */
