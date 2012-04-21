#include "IImageBase.h"
#include "utils/PixelFormatUtils.h"

IImageBase::IImageBase( const ImageDescription& imageDescription )
	: m_Description( imageDescription ),
	m_Pixels( m_Description.width * m_Description.height * (m_Description.depth==0?1:m_Description.depth) * bytesPerPixel( m_Description.format ) ) {}

IImageBase::~IImageBase() {}

const ImageDescription& IImageBase::getImageDescription() const
{
	return m_Description;
}

