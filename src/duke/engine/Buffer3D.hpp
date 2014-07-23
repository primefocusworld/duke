#pragma once
#include <fstream>
#include <sstream>
#include <cstdio> // sscanf
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include "duke/gl/GlObjects.hpp"


namespace duke {

struct Buffer3D {
	
  typedef float ComponentType;// GL_UNSIGNED_BYTE or GL_FLOAT ?
  ComponentType *rawBuffer;
  unsigned int nbChannel;
  unsigned int nbSamplePerChannel;    
 
  Buffer3D(uint size);
  ~Buffer3D();
  void uploadTo(gl::GlTexture3D  &tex, uint textID);
  void createIDLut(uint size, ComponentType *rawBuffer);

};

};//namespace duke

