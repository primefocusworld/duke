#ifndef FACTORIES_H_
#define FACTORIES_H_

#include "resource/IMeshBase.h"
#include "resource/IShaderBase.h"
#include "resource/IImageBase.h"
#include "resource/ITextureBase.h"

#include <dukeio/ImageDescription.h>

#include <string>

namespace duke {namespace protocol {
class Texture;
class Shader;
class Mesh;
}}
namespace resource {
struct ResourceCache;
}
class IRenderer;

ImageDescription getImageDescriptionFrom(const ::duke::protocol::Texture& texture);

void dump(const IImageBase* pImage, const std::string& filename);
void dump(const IImageBase* pImage, ::duke::protocol::Texture &texture);

void putImage(resource::ResourceCache& cache, const ::duke::protocol::Texture& texture);

ITextureBase* buildTexture(IRenderer& renderer, const IImageBase* pImage);
IShaderBase* buildShader(IRenderer& renderer, const ::duke::protocol::Shader& shader, const TShaderType type);
IMeshBase* buildMesh(IRenderer& renderer, const ::duke::protocol::Mesh& mesh);

TexturePtr getNamedTexture(IRenderer& renderer, const std::string& imageName);
MeshPtr getNamedMesh(IRenderer& renderer, const std::string& meshName);
ShaderPtr getNamedShader(IRenderer& renderer, const std::string& shaderName, const TShaderType& type);

#endif // FACTORIES_H_
