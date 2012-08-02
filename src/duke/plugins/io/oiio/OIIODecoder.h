#ifndef OIIODECODER_H_
#define OIIODECODER_H_

#include <dukeio/ImageDescription.h>
#include <dukeplugin/IBoostPlugin.h>
#include <dukeplugin/suite/property/PropertySuiteImpl.h>
#include <boost/thread/tss.hpp>

#include <OpenImageIO/imageio.h>

OIIO_PLUGIN_NAMESPACE_BEGIN //
class ImageInput;
OIIO_PLUGIN_NAMESPACE_END //

class OIIODecoder : public IBoostPlugin {
public:
    typedef OIIO_NAMESPACE::OIIO_VERSION_NS::ImageInput ImageInput;
    typedef boost::thread_specific_ptr<ImageInput> ImageInputPtr;

    OIIODecoder();
    virtual ~OIIODecoder();

public:
    OfxStatus noOp();
    OfxStatus describe(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out);
    OfxStatus readHeader(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out);
    OfxStatus decodeImage(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out);

private:
    OfxStatus safeClose();
    void safeSet(ImageInput *pImage);
    int getImageFormat(const OIIO_NAMESPACE::OIIO_VERSION_NS::ImageSpec& imagespec) const;
    void printDebug(const OIIO_NAMESPACE::OIIO_VERSION_NS::ImageSpec& imagespec);

private:
    ImageInputPtr m_pImageInput;
    openfx::plugin::PropertySuiteImpl m_PropertySuite;

};

#endif /* OIIODECODER_H_ */
