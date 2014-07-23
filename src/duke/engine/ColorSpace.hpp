#pragma once

namespace duke {

enum class ColorSpace {
  linear,
    sRGB,
    sRGBf,
    rec709,
    Cineon,
    Panalog,
    REDLog,
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
const char* getColorspaceString(ColorSpace &colorspace);

} /* namespace duke */
