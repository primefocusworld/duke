#include "ITextureBase.h"

ITextureBase::ITextureBase( const ImageDescription& description, const TPixelFormat texturePixelFormat, long usageFlag )
	: m_Description( description ),
	m_TexturePixelFormat( texturePixelFormat ),
	m_iUsageFlag( usageFlag ) {}

ITextureBase::~ITextureBase() {}



