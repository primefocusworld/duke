#ifndef IIMAGEBASE_H_
#define IIMAGEBASE_H_

#include "IResource.h"
#include <dukeengine/renderer/IImageDescriptor.h>
#include <dukeengine/renderer/Enums.h>
#include <dukeio/ImageDescription.h>
#include <vector>

class IImageBase : public IResource, public IImageDescriptor {
    friend class Image;
    friend class DisplayableImage;

public:
    IImageBase(const ImageDescription&);
    virtual ~IImageBase();

    virtual const ImageDescription& getImageDescription() const;
private:
    const ImageDescription m_Description;
    typedef std::vector<unsigned char> Pixels;
    Pixels m_Pixels;
};

#endif /* IIMAGEBASE_H_ */
