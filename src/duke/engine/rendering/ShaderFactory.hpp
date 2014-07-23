#pragma once

#include <duke/gl/Program.hpp>
#include <duke/engine/ColorSpace.hpp>
#include <duke/OpenColorIO/OpenColorIOManager.hpp>

namespace duke {

struct ShaderDescription {
  // vertex
  // fragment
  bool sampleTexture = true;
  bool displayUv = false;
  // texture
  bool grayscale = false;
  bool swapEndianness = false;
  bool swapRedAndBlue = false;
  bool tenBitUnpack = false;
  ColorSpace fileColorspace = ColorSpace::linear;    // aka input colorspace
  ColorSpace screenColorspace = ColorSpace::linear;  // aka output colorspace
 
  std::string OCIOoutput;

  ShaderDescription() = default;
  bool operator<(const ShaderDescription &other) const;

  static ShaderDescription createTextureDesc(bool grayscale, bool swapEndianness, bool swapRedAndBlue,
                                             bool tenBitUnpack, ColorSpace fileColorspace, ColorSpace screenColorspace, std::string OCIOoutput);
  static ShaderDescription createSolidDesc();
  static ShaderDescription createUvDesc();
};

std::string buildFragmentShaderSource(const ShaderDescription &description);
std::string buildVertexShaderSource(const ShaderDescription &description);
SharedProgram buildProgram(const ShaderDescription &description);

} /* namespace duke */
