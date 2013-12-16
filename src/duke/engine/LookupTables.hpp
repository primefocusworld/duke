#pragma once

// First implementation of a lookup transform.

#include "duke/gl/GlObjects.hpp"
#include <string>

namespace duke {

struct Context;

// Lookup table data
struct LookupTransform {
	//
	LookupTransform();
    gl::GlTexture1D lookup1d; // pre-lut
    gl::GlTexture3D	lookup3d; // 3d lut
    float lookup1d_min, lookup1d_max;
    int lut1DSize, lut3DSize;

};

LookupTransform* createIDLut(uint size = 32);
LookupTransform* createFromFile(const std::string &);
LookupTransform* defaultsRGB(); // defaut for testing purposes


};//namespace duke
