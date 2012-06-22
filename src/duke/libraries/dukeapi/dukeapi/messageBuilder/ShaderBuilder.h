#ifndef SHADERBUILDER_H_
#define SHADERBUILDER_H_

#include "details/IBuilder.h"
#include "details/SimpleShaders.h"
#include <player.pb.h>
#include <iostream>

namespace duke {
namespace protocol {

class ShaderBuilder : public IBuilder {
public:
    const static std::string default_vertex_shader;
    const static std::string default_pixel_shader;
    static Shader vertexShader(std::string name, const char* code = NULL, std::string scopeName = "");
    static Shader pixelShader(std::string name, const char* code = NULL, std::string scopeName = "");
    static void addShadingNode(Shader & ps, std::string prototypename, size_t operand, size_t funcID = 0);
    virtual boost::shared_ptr<google::protobuf::Message> build(const std::string name);
};

} // namespace protocol
} // namespace duke

#endif // SHADERBUILDER_H_
