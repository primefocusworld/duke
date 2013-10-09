#pragma once

// First implementation of a lookup transform.

#include "duke/gl/GlObjects.hpp"

namespace duke {

// Lookup table data
struct LookupTransform {
	//
	LookupTransform();
	//gl::GlTexture1D lookup1d; // pre-lut
	gl::GlTexture3D	lookup3d; // 3d lut
};

LookupTransform* createFromFile(const char *);
LookupTransform* defaultsRGB(); // defaut for testing purposes


};//namespace duke
