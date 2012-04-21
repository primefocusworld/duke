#ifndef SHADERFACTORY_H_
#define SHADERFACTORY_H_

#include "IShaderBase.h"

namespace duke {namespace protocol {class Shader;}}
class IRenderer;

ShaderPtr build(IRenderer& renderer, const ::duke::protocol::Shader& shader, const TShaderType type);

#endif /* SHADERFACTORY_H_ */
