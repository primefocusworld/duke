#ifndef IIMAGEBASE_H_
#define IIMAGEBASE_H_

#include "ResourceCache.h"
#include "IImageDescriptor.h"
#include "Enums.h"
#include <dukeio/ImageDescription.h>
#include <vector>

class IImageBase : public ::resource::IResource, public IImageDescriptor {
    friend class Image;
    friend class DisplayableImage;

public:
    IImageBase(const ImageDescription&);
    virtual ~IImageBase();

    virtual const ImageDescription& getImageDescription() const;
private:
    const ImageDescription m_Description;
    typedef std::vector<unsigned char> VECTOR;
    VECTOR m_Pixels;
};

#endif /* IIMAGEBASE_H_ */
