#ifndef SHADERFACTORY_H_
#define SHADERFACTORY_H_

#include <dukeengine/renderer/resource/IShaderBase.h>

namespace duke {namespace protocol {class Shader;}}
class IRenderer;

IShaderBase* buildShader(IRenderer& renderer, const ::duke::protocol::Shader& shader, const TShaderType type);

#endif /* SHADERFACTORY_H_ */
