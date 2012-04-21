#ifndef IMAGE_H_
#define IMAGE_H_

#include "Enums.h"
#include "IImageDescriptor.h"
#include "IImageBase.h"
#include <boost/shared_ptr.hpp>
#include <string>

class IRenderer;

class Image : public IImageDescriptor {
private:
    friend class DisplayableImage;

public:
    // create or update an image from ImageDescription
    Image(IRenderer& renderer, const std::string& name, const ImageDescription& imageDescription);

    // fetch registered image with name
    Image(IRenderer& renderer, const std::string& name);

    void dump(const std::string& filename) const;
    void dump(::duke::protocol::Texture &texture) const;

    virtual ~Image();

    virtual const ImageDescription& getImageDescription() const;

protected:
    ::boost::shared_ptr<IImageBase> m_pImage;
};

#endif // IMAGE_H_
