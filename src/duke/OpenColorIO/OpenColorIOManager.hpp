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

//#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <duke/engine/Buffer3D.hpp>
//#include <GL/glut.h>

#include <OpenColorIO/OpenColorIO.h>
namespace OCIO = OCIO_NAMESPACE;

#include <OpenImageIO/imageio.h>
#include <OpenImageIO/typedesc.h>
#if (OIIO_VERSION < 10100)
namespace OIIO = OIIO_NAMESPACE;
#endif

//#ifndef OPENCOLORIOMANAGER_H
//#define OPENCOLORIOMANAGER_H

using namespace std;
OCIO_NAMESPACE_USING;

namespace duke {

  class OpenColorIOManager {

public:
   OpenColorIOManager(std::string ColorspaceString, std::string lutFilePath);
  ~OpenColorIOManager();
  
    void SetState();
    void CreateGPUShaderDesc();
    void Compute3DLut();
    std::string ComputeShader();
   
    std::string output;
    gl::GlTexture3D m_tex;
    bool flag_raw;
  
private:
  void Initialize(std::string ColorspaceString, std::string lutFilePath);
    
    OCIO::ConstConfigRcPtr config;
    OCIO::DisplayTransformRcPtr transform;
    OCIO::FileTransformRcPtr filetransform;

    std::string inputColorSpace;
    std::string display;
    std::string transformName;
    std::string lutFilePath;
  
    GLuint program;
    int LUT3D_EDGE_SIZE;
    std::string lut3dcacheid;
    std::string shadercacheid;
    OCIO::ConstProcessorRcPtr processor;
    OCIO::GpuShaderDesc shaderDesc;
    GLuint lut3dTexID;
    Buffer3D *Buf3D;

    //float exposure_fstop;
    //float display_gamma;
    //int channelHot[4]; 
  };
  
};

//#endif // OPENCOLORIOMANAGER_H
