#pragma once

namespace duke {

enum class ColorSpace {
  linear,
    sRGB,
    sRGBf,
    rec709,
    Cineon,
    Panalog,
    REDlog,
    ViperLog,
    AlexaV3LogC,
    PLogLin, 
    SLog, 
    raw,
    Gamma18,
    Gamma22
};

// Deduce a the colorspace
ColorSpace resolveFromExtension(const char* pFileExtension);
ColorSpace resolveFromName(const char* pColorspace);

// GlSl functions names
//const char* getToLinearFunction(const ColorSpace fromColorspace);
  //const char* getToScreenFunction(const ColorSpace fromColorspace);

} /* namespace duke */
