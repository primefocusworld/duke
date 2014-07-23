#include "duke/engine/ColorSpace.hpp"
#include "duke/base/StringUtils.hpp"

#include <stdexcept>
#include <set>
#include <string>

namespace duke {

ColorSpace resolveFromName(const char* pColorspace) {
  if (pColorspace) {
    if (streq(pColorspace, "linear")) return ColorSpace::linear;
    if (streq(pColorspace, "sRGB") || streq(pColorspace, "GammaCorrected")) return ColorSpace::sRGB;
    if (streq(pColorspace, "Cineon")) return ColorSpace::Cineon;
    if (streq(pColorspace, "rec709")) return ColorSpace::rec709;
    if (streq(pColorspace, "AlexaV3LogC")) return ColorSpace::AlexaV3LogC;
    if (streq(pColorspace, "sRGBf")) return ColorSpace::sRGBf;
    if (streq(pColorspace, "Gamma1.8")) return ColorSpace::Gamma18;
    if (streq(pColorspace, "Gamma2.2")) return ColorSpace::Gamma22;
    if (streq(pColorspace, "Panalog")) return ColorSpace::Panalog;
    if (streq(pColorspace, "REDLog")) return ColorSpace::REDLog;
    if (streq(pColorspace, "ViperLog")) return ColorSpace::ViperLog;
    if (streq(pColorspace, "PLogLin")) return ColorSpace::PLogLin;
    if (streq(pColorspace, "SLog")) return ColorSpace::SLog;
    if (streq(pColorspace, "raw")) return ColorSpace::raw;
    
  }
  return ColorSpace::linear;
}

ColorSpace resolveFromExtension(const char* pFileExtension) {
  if (pFileExtension) {
    if (streq(pFileExtension, "dpx")) return ColorSpace::Cineon;
    if (streq(pFileExtension, "exr")) return ColorSpace::linear;
    if (streq(pFileExtension, "jpg")) return ColorSpace::sRGB;
    if (streq(pFileExtension, "png")) return ColorSpace::sRGB;
  }
  static std::set<std::string> reported;
  if (reported.find(pFileExtension) == reported.end()) {
    printf("Unable to find default ColorSpace for extension '%s' assuming sRGB\n", pFileExtension);
    reported.insert(pFileExtension);
  }
  return ColorSpace::sRGB;
}

 const char* getColorspaceString(ColorSpace &colorspace) {
    switch (colorspace) {
    case ColorSpace::linear:
        return "linear";
    case ColorSpace::sRGB:
        return "sRGB";
    case ColorSpace::sRGBf:
        return "sRGBf";
    case ColorSpace::rec709:
        return "rec709";
    case ColorSpace::Cineon:
        return "Cineon";
    case ColorSpace::Panalog:
        return "Panalog";
    case ColorSpace::REDLog:
        return "REDLog";
    case ColorSpace::ViperLog:
        return "ViperLog";
    case ColorSpace::AlexaV3LogC:
        return "AlexaV3LogC";
    case ColorSpace::PLogLin:
        return "PLogLin";
    case ColorSpace::SLog:
        return "SLog";
    case ColorSpace::raw:
        return "raw";
    case ColorSpace::Gamma18:
        return "Gamma1.8";
    case ColorSpace::Gamma22:
        return "Gamma2.2";
    }
    throw std::runtime_error("unknown colorspace");
    }

  
}  // namespace duke
