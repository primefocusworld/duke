#pragma once

// First implementation of a lookup transform.

#include "duke/gl/GlObjects.hpp"
#include <string>

namespace duke {

struct Context;

std::vector<float> g_lut3d;
void AllocateLut3D();
void InitOCIO(unsigned char ColorSpace);
void UpdateOCIOGLState();


};//namespace duke
