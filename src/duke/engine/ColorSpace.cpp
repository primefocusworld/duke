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
  /*
// GLSL ColorSpace conversion functions
const char* pColorSpaceConversions = R"(
vec3 lintolin(vec3 sample) {
	return sample;
}
vec3 alexatolin(vec3 sample) {
    return mix((pow(vec3(10.0),((sample-0.385537)/0.2471896))-0.052272)/5.555556 , (sample-0.092809)/5.367655, lessThan(sample, vec3(0.1496582)));
}
vec3 cineontolin(vec3 sample) {
	return 1.010915615730753*(pow(vec3(10), (1023*sample-685)/300)-0.010797751623277);
}
vec3 srgbtolin(vec3 sample) {
    return mix(pow((sample+0.055)/1.055,vec3(2.4)), sample/12.92, lessThan(sample, vec3(0.04045)));
}
vec3 lintosrgb(vec3 sample) {
    sample = mix((1.055*pow(sample,vec3(1/2.4)))-vec3(0.055), 12.92*sample, lessThan(sample, vec3(0.0031308)));
    return clamp(sample, vec3(0), vec3(1));
}
vec3 rec709tolin(vec3 sample) {
    return mix(pow((sample+0.099)/1.099,vec3(1/0.45)), sample/4.5, lessThan(sample, vec3(0.018)));
}
vec3 lintorec709(vec3 sample) {
    sample = mix((1.099*pow(sample, vec3(0.45)))-vec3(0.099), 4.5*sample, lessThan(sample, vec3(0.018)));
    return clamp(sample, vec3(0), vec3(1));
}

)";
  
const char* getToLinearFunction(const ColorSpace fromColorspace) {
  switch (fromColorspace) {
    case ColorSpace::AlexaV3LogC:
      return "alexatolin";
    case ColorSpace::Cineon:
      return "cineontolin";
    case ColorSpace::sRGB:
    case ColorSpace::Gamma18:
      return "srgbtolin";
    case ColorSpace::rec709:
      return "rec709tolin";
    case ColorSpace::linear:
    default:
      throw std::runtime_error("ColorSpace must be resolved at this point");
  }
}

const char* getToScreenFunction(const ColorSpace fromColorspace) {
  switch (fromColorspace) {
    case ColorSpace::rec709:
      return "lintorec709";
    case ColorSpace::sRGB:
    case ColorSpace::Gamma18:
    case ColorSpace::linear:
      return "lintosrgb";  // this is the default screen colorspace though
    default:
      throw std::runtime_error("Screen colorspace not handled");
  }
}
  */
}  // namespace duke
