#pragma once
#include <duke/OpenColorIO/OpenColorIOManager.hpp>

namespace duke {

class Mesh;
struct Context;
struct ShaderPool;
void renderWithBoundTexture(const ShaderPool &shaderPool, const Mesh *pMesh,  const Context &context, std::string OCIOoutput, bool OCIOflag_raw);
float getZoomValue(const Context &context);

} /* namespace duke */
