#pragma once

namespace duke {

class Mesh;
struct Context;
struct ShaderPool;

void renderWithBoundTexture(const ShaderPool &shaderPool, const Mesh *pMesh, int lutSize, const Context &context);

float getZoomValue(const Context &context);

} /* namespace duke */
