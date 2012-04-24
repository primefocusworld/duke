#ifndef IIMAGEBASE_H_
#define IIMAGEBASE_H_

#include "IResource.h"
#include <dukeengine/renderer/Enums.h>
#include <dukeio/ImageDescription.h>
#include <vector>

class IImageBase : public IResource {
    friend class Image;
    friend class DisplayableImage;

public:
    IImageBase(const ImageDescription&);
    virtual ~IImageBase();

    virtual const ImageDescription& getImageDescription() const;

    typedef std::vector<unsigned char> Pixels;
    const Pixels& pixels() const;
private:
    const ImageDescription m_Description;
    Pixels m_Pixels;
};

#include <boost/shared_ptr.hpp>
typedef ::boost::shared_ptr<IImageBase> ImagePtr;

#endif /* IIMAGEBASE_H_ */
