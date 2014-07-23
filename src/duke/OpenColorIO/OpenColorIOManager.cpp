#include "OpenColorIOManager.hpp"

namespace duke {

  OpenColorIOManager::OpenColorIOManager(std::string ColorspaceString, std::string lutFilePath)
{  
  Initialize(ColorspaceString, lutFilePath);
}

  void OpenColorIOManager::Initialize(std::string ColorspaceString, std::string lutFilePath) 
{ 
  this->config = OCIO::GetCurrentConfig();
   if (lutFilePath != "")
  {
  this->filetransform = OCIO::FileTransform::Create();
  }
  else { this->transform = OCIO::DisplayTransform::Create();}

  this->display = config->getDefaultDisplay();
  this->transformName = config->getDefaultView(this->display.c_str());
  this->inputColorSpace = ColorspaceString;
  this->lutFilePath= lutFilePath;

  //std::cout << "Initializing transform Name: "<<  this->transformName.c_str() << std::endl;
  //std::cout << "Initializing input Color Space: "<<  this->inputColorSpace.c_str() << std::endl;
  // std::cout << "Initializing Display: "<< this->display.c_str() << std::endl;

  this->program = 0;
  this->lut3dTexID = 2;
  this->LUT3D_EDGE_SIZE = 32;
  this-> Buf3D = new Buffer3D(LUT3D_EDGE_SIZE);

  if (ColorspaceString=="raw")
    {
      this->flag_raw = true;
    }
  else 
    {flag_raw = false;}

  //IF WE EVER NEED TO CHANGE THE EXPOSURE AND GAMMA THROUGH OCIO
   //this->channelHot[0] = 1;  
  //this->channelHot[1] = 1;   
  //this->channelHot[2] = 1;   
  //this->channelHot[3] = 1;
  //this->exposure_fstop = 0.0f;
  //this->display_gamma = 1.0f;


}

OpenColorIOManager::~OpenColorIOManager()
{
  delete Buf3D;
}

void OpenColorIOManager::SetState()
{
   if (lutFilePath != "")
     {
       //std::cout << "SetState LutFilePath " <<lutFilePath <<std::endl;
     filetransform->setSrc(lutFilePath.c_str());
     filetransform->setInterpolation(OCIO::INTERP_BEST);

    try
    {
        processor = config->getProcessor(filetransform);
    }
    catch(OCIO::Exception & e)
    {
        std::cerr << e.what() << std::endl;
        std::cout << "SetState"  <<std::endl;
        return;

    }
    catch(...)
    {   return;

    }

  }
  
  else
    { //std::cout <<" Step 0: Get the processor using any of the pipelines mentioned above (LutFilePath)" <<std::endl;
  OCIO::DisplayTransformRcPtr transform = OCIO::DisplayTransform::Create();
  transform->setInputColorSpaceName(inputColorSpace.c_str() );
  transform->setDisplay( display.c_str() );
  transform->setView( transformName.c_str() );

    /*
   //IF WE EVER NEED TO CHANGE THE EXPOSURE AND GAMMA THROUGH OCIO
       // Fstop exposure control (in SCENE_LINEAR)

       {
           float gain = powf(2.0f, exposure_fstop);
           const float slope4f[] = { gain, gain, gain, gain };
           float m44[16];
           float offset4[4];
           OCIO::MatrixTransform::Scale(m44, offset4, slope4f);
           OCIO::MatrixTransformRcPtr mtx =  OCIO::MatrixTransform::Create();
           mtx->setValue(m44, offset4);
           transform->setLinearCC(mtx);
       }

       // Channel swizzling
       {
           float lumacoef[3];
           config->getDefaultLumaCoefs(lumacoef);
           float m44[16];
           float offset[4];
           OCIO::MatrixTransform::View(m44, offset, channelHot, lumacoef);
           OCIO::MatrixTransformRcPtr swizzle = OCIO::MatrixTransform::Create();
           swizzle->setValue(m44, offset);
           transform->setChannelView(swizzle);
       }

       // Post-display transform gamma
       {
           float exponent = 1.0f/std::max(1e-6f, static_cast<float>(display_gamma));
           const float exponent4f[] = { exponent, exponent, exponent, exponent };
           OCIO::ExponentTransformRcPtr expTransform =  OCIO::ExponentTransform::Create();
           expTransform->setValue(exponent4f);
           transform->setDisplayCC(expTransform);
       }

  */

    try
    {
        processor = config->getProcessor(transform);
    }
    catch(OCIO::Exception & e)
    {
        std::cerr << e.what() << std::endl;
        std::cout << "SetState"  <<std::endl;
        return;

    }
    catch(...)
    {   return;

    }
 }


}


void OpenColorIOManager::CreateGPUShaderDesc()
{

    // Step 1: Create a GPU Shader Description
    shaderDesc.setLanguage(OCIO::GPU_LANGUAGE_GLSL_1_0);
    shaderDesc.setFunctionName("OCIODisplay");
    shaderDesc.setLut3DEdgeLen(LUT3D_EDGE_SIZE);

 
}

void OpenColorIOManager::Compute3DLut()
{
  
  // Step 2: Compute the 3D LUT
    std::string lut3dCacheID = processor->getGpuLut3DCacheID(shaderDesc);
    if(lut3dCacheID != lut3dcacheid)
    {
    
        lut3dcacheid = lut3dCacheID;
	
	processor->getGpuLut3D(&Buf3D->rawBuffer[0], shaderDesc);
	//std::cout << m_tex.id << " Here is compute3D Lut  " << m_tex.target << std::endl;

	Buf3D->uploadTo(m_tex,lut3dTexID );			
     }

}

std::string OpenColorIOManager::ComputeShader()
{
  
    // Step 3: Compute the Shader
    std::string shaderCacheID = processor->getGpuShaderTextCacheID(shaderDesc);
    
    if(program == 0 || shaderCacheID != shadercacheid)
    {
      //std::cerr << "Computing Shader " << shadercacheid << std::endl;

        shadercacheid = shaderCacheID;

	 std::ostringstream os;
	os << processor->getGpuShaderText(shaderDesc) << "\n";

        output = os.str();
	//Remplace texture3D by texture
	std::string goaway = "texture3D";
	std::size_t pos = output.find(goaway); 
	if (pos!= string::npos)
	  {	
	    output = output.replace(pos, goaway.length(), "texture"); 
	  }
	
	}
    return output;
}


};//namespace duke


