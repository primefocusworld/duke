/*
 * ShaderBuilder.cpp
 *
 *  Created on: 12 juin 2012
 *      Author: Nicolas Rondaud
 */

#include "ShaderBuilder.h"
#include <boost/make_shared.hpp>

namespace duke {
namespace protocol {

const std::string ShaderBuilder::default_vertex_shader("vs");
const std::string ShaderBuilder::default_pixel_shader("ps");

Shader ShaderBuilder::vertexShader(std::string name, const char* code, std::string scopeName) {
    Shader shader;
    if (!scopeName.empty())
        name += "_" + scopeName;
    shader.set_name(name);
    if (code != NULL)
        shader.set_code(code);
    return shader;
}

Shader ShaderBuilder::pixelShader(std::string name, const char* code, std::string scopeName) {
    Shader shader;
    if (!scopeName.empty())
        name += "_" + scopeName;
    shader.set_name(name);
    if (code != NULL)
        shader.set_code(code);
    else {
        Program * p = shader.mutable_program();
        // main
        FunctionInstance * f = p->add_function();
        FunctionInstance_CallGraph * g = f->mutable_callgraph();
        FunctionSignature * sign = g->mutable_signature();
        sign->set_name("main");
        sign->set_returntype("float4");
        sign->set_varyingouputsemantic("COLOR");
        sign->add_operandtype("PixelInput");
        sign->add_prependdeclaration("struct PixelInput{float2 uv0: TEXCOORD0;};");

        FunctionInstance_CallGraph_FunctionCall * call0 = g->add_call();
        FunctionInstance * f0 = call0->mutable_function();
        f0->set_inlinedvalue("@0.uv0");

        FunctionInstance_CallGraph_FunctionCall * call1 = g->add_call();
        FunctionInstance * f1 = call1->mutable_function();
        f1->set_useprototypenamed("sample2d");
        f1->add_parametername("sampler");
        call1->add_operand(0);
    }
    return shader;
}

void ShaderBuilder::addShadingNode(Shader & ps, std::string prototypename, size_t operand, size_t funcID) {
    // retrieve CallGraph...
    Program * p = ps.mutable_program();
    FunctionInstance * f = p->mutable_function(funcID);
    FunctionInstance_CallGraph * g = f->mutable_callgraph();
    // .. and add a call
    FunctionInstance_CallGraph_FunctionCall * call0 = g->add_call();
    FunctionInstance * f0 = call0->mutable_function();
    f0->set_useprototypenamed(prototypename);
    call0->add_operand(operand);
}

boost::shared_ptr<google::protobuf::Message> ShaderBuilder::build(const std::string name) {
    if (name == ShaderBuilder::default_vertex_shader)
        return boost::make_shared < Shader > (vertexShader(ShaderBuilder::default_vertex_shader));
    if (name == ShaderBuilder::default_pixel_shader)
        return boost::make_shared < Shader > (vertexShader(ShaderBuilder::default_pixel_shader));
    return boost::shared_ptr<Shader>();
}

} // namespace protocol
} // namespace duke
