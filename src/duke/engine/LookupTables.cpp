#include <fstream>
#include <sstream>
#include <cstdio> // sscanf
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>


//#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
//#include <GL/glut.h>

#include <OpenColorIO/OpenColorIO.h>
namespace OCIO = OCIO_NAMESPACE;

#include <OpenImageIO/imageio.h>
#include <OpenImageIO/typedesc.h>
#if (OIIO_VERSION < 10100)
namespace OIIO = OIIO_NAMESPACE;
#endif

namespace duke {

GLuint g_fragShader = 0;
GLuint g_program = 0;

GLuint g_lut3dTexID;
const int LUT3D_EDGE_SIZE = 32;
std::vector<float> g_lut3d;
std::string g_lut3dcacheid;
std::string g_shadercacheid;

std::string g_inputColorSpace;
std::string g_display;
std::string g_transformName;

#include "duke/engine/LookupTables.hpp"



void AllocateLut3D()
{
    glGenTextures(1, &g_lut3dTexID);

    int num3Dentries = 3*LUT3D_EDGE_SIZE*LUT3D_EDGE_SIZE*LUT3D_EDGE_SIZE;
    g_lut3d.resize(num3Dentries);
    memset(&g_lut3d[0], 0, sizeof(float)*num3Dentries);

    glActiveTexture(GL_TEXTURE2);

    glBindTexture(GL_TEXTURE_3D, g_lut3dTexID);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16F_ARB,
                 LUT3D_EDGE_SIZE, LUT3D_EDGE_SIZE, LUT3D_EDGE_SIZE,
                 0, GL_RGB,GL_FLOAT, &g_lut3d[0]);
}



  void InitOCIO(std::string ColorSpace)
{
    OCIO::ConstConfigRcPtr config = OCIO::GetCurrentConfig();
    g_display = config->getDefaultDisplay();
    g_transformName = config->getDefaultView(g_display.c_str());

    g_inputColorSpace = OCIO::ROLE_SCENE_LINEAR;
    
        //std::string cs = config->parseColorSpaceFromString(filename);
      //std::string cs = config->getColorSpaceNameByIndex(index);
      std::string cs = ColorSpace;


        // std::string cs = "lnf";
        if(!cs.empty())
        {
            g_inputColorSpace = cs;
            std::cout << "colorspace: " << cs << std::endl;
        }
        else
        {
            std::cout << "colorspace: " << g_inputColorSpace << " \t(could not determine from filename, using default)" << std::endl;
        }
    

    //g_inputColorSpace = ColorSpace;
    //std::cout << "colorspace: " << g_inputColorSpace << std::endl;

}

GLuint
CompileShaderText(GLenum shaderType, const char *text)
{
    GLuint shader;
    GLint stat;

    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, (const GLchar **) &text, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &stat);

    if (!stat)
    {
        GLchar log[1000];
        GLsizei len;
        glGetShaderInfoLog(shader, 1000, &len, log);
        fprintf(stderr, "Error: problem compiling shader: %s\n", log);
        return 0;
    }

    return shader;
}
 
GLuint
LinkShaders(GLuint fragShader)
{
    if (!fragShader) return 0;

    GLuint program = glCreateProgram();

    if (fragShader)
        glAttachShader(program, fragShader);

    glLinkProgram(program);

    // check link
    {
        GLint stat;
        glGetProgramiv(program, GL_LINK_STATUS, &stat);
        if (!stat) {
            GLchar log[1000];
            GLsizei len;
            glGetProgramInfoLog(program, 1000, &len, log);
            fprintf(stderr, "Shader link error:\n%s\n", log);
            return 0;
        }
    }

    return program;
}


const char * g_fragShaderText = ""
                                "\n"
                                "uniform sampler2D lookup2d;\n"
                                "uniform sampler3D lookup3d;\n"
                                "\n"
                                "void main()\n"
                                "{\n"
                                "    vec4 col = texture2D(lookup2d, gl_TexCoord[0].st);\n"
                                "    gl_FragColor = OCIODisplay(col, lookup3d);\n"
                                "}\n";


void UpdateOCIOGLState()
{
    // Step 0: Get the processor using any of the pipelines mentioned above.
    OCIO::ConstConfigRcPtr config = OCIO::GetCurrentConfig();

    OCIO::DisplayTransformRcPtr transform = OCIO::DisplayTransform::Create();

    transform->setInputColorSpaceName( g_inputColorSpace.c_str() );
    transform->setDisplay( g_display.c_str() );
    transform->setView( g_transformName.c_str() );

    std::cout << "Transform Name: "<<  g_transformName.c_str() << std::endl;
    std::cout << "input Color Space: "<<  g_inputColorSpace.c_str() << std::endl;
    std::cout << "Display blah: "<< g_display.c_str() << std::endl;


    OCIO::ConstProcessorRcPtr processor;
    try
    {
        processor = config->getProcessor(transform);
    }
    catch(OCIO::Exception & e)
    {
        std::cerr << e.what() << std::endl;
        std::cout << "UpdateOCIOGLState"  <<std::endl;
        return;

    }
    catch(...)
    {   return;

    }

    // Step 1: Create a GPU Shader Description
    OCIO::GpuShaderDesc shaderDesc;
    shaderDesc.setLanguage(OCIO::GPU_LANGUAGE_GLSL_1_0);
    shaderDesc.setFunctionName("OCIODisplay");
    shaderDesc.setLut3DEdgeLen(LUT3D_EDGE_SIZE);

    // Step 2: Compute the 3D LUT
    std::string lut3dCacheID = processor->getGpuLut3DCacheID(shaderDesc);
    if(lut3dCacheID != g_lut3dcacheid)
    {
        std::cerr << "Computing 3DLut HERRE  " << g_lut3dcacheid << std::endl;

        g_lut3dcacheid = lut3dCacheID;
        processor->getGpuLut3D(&g_lut3d[0], shaderDesc);



        glBindTexture(GL_TEXTURE_3D, g_lut3dTexID);
        glTexSubImage3D(GL_TEXTURE_3D, 0,
                        0, 0, 0,
                        LUT3D_EDGE_SIZE, LUT3D_EDGE_SIZE, LUT3D_EDGE_SIZE,
                        GL_RGB,GL_FLOAT, &g_lut3d[0]);
    }

    // Step 3: Compute the Shader
    std::string shaderCacheID = processor->getGpuShaderTextCacheID(shaderDesc);
    if(g_program == 0 || shaderCacheID != g_shadercacheid)
    {
        std::cerr << "Computing Shader TESSST " << g_shadercacheid << std::endl;

        g_shadercacheid = shaderCacheID;

        std::ostringstream os;
        os << processor->getGpuShaderText(shaderDesc) << "\n";


	//        os << g_fragShaderText;
        std::cerr << os.str() << std::endl;

        if(g_fragShader) glDeleteShader(g_fragShader);
        g_fragShader = CompileShaderText(GL_FRAGMENT_SHADER, os.str().c_str());
        if(g_program) glDeleteProgram(g_program);
        g_program = LinkShaders(g_fragShader);
    }

    //glUseProgram(g_fragShader);
    glUseProgram(g_program);
    glUniform1i(glGetUniformLocation(g_program, "lookup2d"), 1);
    glUniform1i(glGetUniformLocation(g_program, "lookup3d"), 2);


}

void PopulateOCIOMenus()
{
    OCIO::ConstConfigRcPtr config = OCIO::GetCurrentConfig();
    std::cout << "test" << std::endl;
    
    // int csMenuID = glutCreateMenu(imageColorSpace_CB);
    for(int i=0; i<config->getNumColorSpaces(); ++i)
    {

      std::cout <<"ColorSpace: "<< config->getColorSpaceNameByIndex(i)<<std::endl;
      // glutAddMenuEntry(config->getColorSpaceNameByIndex(i), i);
    }
    
    // int deviceMenuID = glutCreateMenu(displayDevice_CB);
    for(int i=0; i<config->getNumDisplays(); ++i)
    {
      std::cout <<"Displays: "<< config->getDisplay(i)<<std::endl;
      // glutAddMenuEntry(config->getDisplay(i), i);
    }
    
    //int transformMenuID = glutCreateMenu(transform_CB);
    //const char * defaultDisplay = config->getDefaultDisplay();
    //for(int i=0; i<config->getNumViews(defaultDisplay); ++i)
    // {
    //    glutAddMenuEntry(config->getView(defaultDisplay, i), i);
    //}
    
    //glutCreateMenu(menuCallback);
    //glutAddSubMenu("Image ColorSpace", csMenuID);
    //glutAddSubMenu("Transform", transformMenuID);
    //glutAddSubMenu("Device", deviceMenuID);
    
    //glutAttachMenu(GLUT_RIGHT_BUTTON);
}



};//namespace duke



