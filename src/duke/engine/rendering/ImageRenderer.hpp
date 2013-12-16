#pragma once

namespace duke {

class Mesh;
struct Context;
struct ShaderPool;

void renderWithBoundTexture(const ShaderPool &shaderPool, const Mesh *pMesh, float lutmin, float lutmax, int lut1dSize, int lutSize,  const Context &context);
float getZoomValue(const Context &context);

} /* namespace duke */
