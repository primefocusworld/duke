#include "ShaderFactory.h"
#include "IRenderer.h"
#include "DisplayableImage.h"
#include "resource/ProtoBufResource.h"
#include <dukeapi/shading/ShadingGraphCodeBuilder.h>

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <google/protobuf/descriptor.h>
#include <sstream>
#include <string>

using namespace ::std;
using namespace ::google::protobuf;
using namespace ::duke::protocol;
using namespace ::shader_factory;

static CGprogram createProgram(IRenderer& renderer, const TShaderType type, const string& code, const string &name) {
    const char** pProgramOptions = renderer.getShaderOptions(type);
    const CGprofile profile = renderer.getShaderProfile(type);
    const CGprogram cgProgram = cgCreateProgram(renderer.getCgContext(), //context
                    CG_SOURCE, // compiling source
                    code.c_str(), // program
                    profile, // CG profile
                    "main", //entry point
                    pProgramOptions //args
                    );

    if (!cgIsProgramCompiled(cgProgram)) {
        cerr << "error while compiling the following code" << endl;
        cerr << "----------------------------------------" << endl;
        cerr << code << endl;
        cerr << "----------------------------------------" << endl;
        cerr << "profile : " << cgGetProfileString(profile) << endl;
        cerr << "options : " << endl;
        for (int i = 0; pProgramOptions[i] != '\0'; ++i)
            cerr << pProgramOptions[i] << endl;
    } else {
#ifdef DEBUG_SHADER
        cout << "========================================" << endl;
        cout << "creating shader" << endl;
        cout << "----------------------------------------" << endl;
        cout << cgGetProgramString(cgProgram, CG_PROGRAM_SOURCE) << endl;
        cout << "----------------------------------------" << endl;
        const string compiled = cgGetProgramString(cgProgram, CG_COMPILED_PROGRAM);
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep("\n");
        tokenizer tokens(compiled, sep);
        size_t instructions = 0;
        for (tokenizer::iterator beg = tokens.begin(); beg != tokens.end(); ++beg, ++instructions)
        if (boost::starts_with(*beg, "//"))
        instructions = 0;
        cout << name << " compiled to " << instructions << " instructions" << endl;
        cout << "========================================" << endl;
#endif
    }
    return cgProgram;
}

inline static std::string getCode(const ::duke::protocol::Shader& shader, const TShaderType type, PrototypeFactory &factory) {
    // shader must have either a shading tree or code
    if (type == SHADER_VERTEX || shader.has_code())
        return shader.code();
    assert(shader.has_program());
    return shader_factory::compile(shader.program(), factory);
}

IShaderBase* buildShader(IRenderer& renderer, const ::duke::protocol::Shader& shader, const TShaderType type) {
    const string code = getCode(shader, type, renderer.prototypeFactory);
    CGprogram program = createProgram(renderer, type, code, shader.name());
    return renderer.createShader(program, type);
}
