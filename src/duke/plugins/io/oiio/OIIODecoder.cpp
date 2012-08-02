#include "OIIODecoder.h"
#include <dukeio/ofxDukeIo.h>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <iostream>

using namespace std;
OIIO_NAMESPACE_USING


static const char* OIIO_FILE_EXTENSIONS = ""
                "bmp,cin,dds,f3d,fits,hdr,ico,iff,jp2,j2k,exr,png,"
                "pbm,pgm,ppm,ptex,rla,sgi,rgb,rgba,bw,int,inta,pic,tga,tpic,"
                "tif,tiff,tx,env,sm,vsm,zfile";

// [x] BMP (*.bmp)
// [x] Cineon (*.cin)
// [x] Direct Draw Surface (*.dds)
// [ ] DPX (*.dpx)
// [x] Field3D (*.f3d)
// [x] FITS (*.fits)
// [x] HDR/RGBE (*.hdr)
// [x] Icon (*.ico)
// [x] IFF (*.iff)
// [ ] JPEG (*.jpg *.jpe *.jpeg *.jif *.jfif *.jfi)
// [x] JPEG-2000 (*.jp2 *.j2k)
// [x] OpenEXR (*.exr)
// [x] Portable Network Graphics (*.png)
// [x] PNM / Netpbm (*.pbm *.pgm *.ppm)
// [x] Ptex (*.ptex)
// [x] RLA (*.rla)
// [x] SGI (*.sgi *.rgb *.rgba *.bw *.int *.inta)
// [x] Softimage PIC (*.pic)
// [x] Targa (*.tga *.tpic)
// [x] TIFF (*.tif *.tiff *.tx *.env *.sm *.vsm)
// [x] Zfile (*.zfile)

OIIODecoder::OIIODecoder() :
    m_pImageInput(NULL), m_PropertySuite(*this) {
    registerAction(kOfxActionLoad, boost::bind(&OIIODecoder::noOp, this));
    registerAction(kOfxActionUnload, boost::bind(&OIIODecoder::noOp, this));
    registerAction(kOfxActionDescribe, boost::bind(&OIIODecoder::describe, this, _1, _2, _3));
    registerAction(kOfxActionCreateInstance, boost::bind(&OIIODecoder::noOp, this));
    registerAction(kOfxActionDestroyInstance, boost::bind(&OIIODecoder::noOp, this));
    registerAction(kOfxDukeIoActionReadHeader, boost::bind(&OIIODecoder::readHeader, this, _1, _2, _3));
    registerAction(kOfxDukeIoActionDecodeImage, boost::bind(&OIIODecoder::decodeImage, this, _1, _2, _3));
}

OIIODecoder::~OIIODecoder() {
}

OfxStatus OIIODecoder::noOp() {
    return kOfxStatOK;
}

OfxStatus OIIODecoder::describe(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    openfx::plugin::PropertyHelper helper = m_PropertySuite.getHelper(OfxPropertySetHandle(handle));
    helper.setString(kOfxDukeIoSupportedExtensions, OIIO_FILE_EXTENSIONS);
    helper.setInt(kOfxDukeIoUncompressedFormat, 0);
    helper.setInt(kOfxDukeIoDelegateRead, 0);
    return kOfxStatOK;
}

OfxStatus OIIODecoder::readHeader(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    try {
        openfx::plugin::PropertyHelper inArgHelper = m_PropertySuite.getHelper(in);
        string filename = inArgHelper.getString(kOfxDukeIoImageFilename);

        // clean reset
        safeClose();
        m_pImageInput.reset(ImageInput::create(filename));
        if (m_pImageInput.get() == NULL)
            return kOfxStatFailed;

        // open image
        ImageSpec spec;
        if (!m_pImageInput->open(filename, spec))
            return safeClose();

#ifdef DEBUG
        // print debug infos
        printDebug(spec);
#endif

        // warn on error
        int format = getImageFormat(spec);
        if (format == kOfxDukeIoImageFormatUndefined) {
            cerr << "OIIO plugin: image format not handled yet.\n";
            return safeClose();
        }

        openfx::plugin::PropertyHelper outArgHelper = m_PropertySuite.getHelper(out);
        outArgHelper.setInt(kOfxDukeIoImageFormat, format);
        outArgHelper.setInt(kOfxDukeIoImageWidth, spec.width);
        outArgHelper.setInt(kOfxDukeIoImageHeight, spec.height);
        outArgHelper.setInt(kOfxDukeIoImageDepth, 0);
        outArgHelper.setInt(kOfxDukeIoBufferSize, spec.image_bytes());
        return kOfxStatOK;

    } catch (exception& e) {
        cerr << "Unhandled exception in OIIO plugin: " << e.what() << endl;
    }
    return safeClose();
}

OfxStatus OIIODecoder::decodeImage(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    try {
        if (m_pImageInput.get() == NULL)
            return kOfxStatFailed;
        openfx::plugin::PropertyHelper inArgHelper = m_PropertySuite.getHelper(in);

        const ImageSpec& spec = m_pImageInput->spec();
        if (spec.nchannels > 4) {
            // read only the 4 first chans
            m_pImageInput->read_scanlines(0, m_pImageInput->spec().height,0,0,4, TypeDesc::UNKNOWN, inArgHelper.getPointer(kOfxDukeIoBufferPtr));
        } else {
            // read the whole image
            m_pImageInput->read_image(TypeDesc::UNKNOWN, inArgHelper.getPointer(kOfxDukeIoBufferPtr));
        }

        safeClose();
        return kOfxStatOK;
    } catch (exception& e) {
        cerr << "Unhandled exception in OIIO plugin: " << e.what() << endl;
    }
    return safeClose();
}

// private
OfxStatus OIIODecoder::safeClose() {
    if (m_pImageInput.get()) {
        m_pImageInput->close();
        m_pImageInput.release();
    }
    return kOfxStatFailed;
}

// private
int OIIODecoder::getImageFormat(const ImageSpec& imagespec) const {

    //TODO oiio image formats
    //  UNKNOWN, NONE, UCHAR, UINT8, INT8, USHORT, SHORT, UINT, INT,
    //  ULONGLONG, LONGLONG, DOUBLE, STRING, PTR, LASTBASE

    int nchannels = imagespec.nchannels;
    if(nchannels > 4) nchannels = 4;

    switch (imagespec.nchannels) {
        case 3: {
            switch (imagespec.format.basetype) {
                case TypeDesc::HALF:
                    return kOfxDukeIoImageFormatB16G16R16F;
                case TypeDesc::FLOAT:
                    return kOfxDukeIoImageFormatB32G32R32F;
                default:
                    return kOfxDukeIoImageFormatB8G8R8;
            }
            break;
        }
        case 4: {
            switch (imagespec.format.basetype) {
                case TypeDesc::HALF:
                    return kOfxDukeIoImageFormatB16G16R16A16F;
                case TypeDesc::FLOAT:
                    return kOfxDukeIoImageFormatB32G32R32A32F;
                default:
                    return kOfxDukeIoImageFormatB8G8R8A8;
            }
            break;
        }
    }
    return kOfxDukeIoImageFormatUndefined;
}

void OIIODecoder::printDebug(const ImageSpec& imagespec) {
    cerr << "-- OIIO Image Spec " << endl;
    cerr << "  * format: " << imagespec.format.c_str() << endl;
    cerr << "  * width: " << imagespec.width << endl;
    cerr << "  * height: " << imagespec.height << endl;
    cerr << "  * alpha_channel index: " << imagespec.alpha_channel << endl;
    cerr << "  * z_channel index: " << imagespec.z_channel << endl;
    cerr << "  * nchannels: " << imagespec.nchannels << endl;
    cerr << "  * channelnames: ";
    {
        BOOST_FOREACH(string s, imagespec.channelnames) {cerr << s << ", ";}
        cerr << endl;
    }
}

//
// Main entry
//
const int PLUGIN_COUNT = 1;

// You should include PluginBootstrap.h only once as it defines global variables and functions
#include <dukeplugin/PluginBootstrap.h>

OfxPluginInstance<0> plug(kOfxDukeIoApi, kOfxDukeIoApiVersion, "fr.mikrosimage.openImageIO", 1, 0, new OIIODecoder());
