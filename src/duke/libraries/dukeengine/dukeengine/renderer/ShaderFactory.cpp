#include "ShaderFactory.h"
#include <dukeapi/shading/ShadingGraphCodeBuilder.h>
#include "ProtoBufResource.h"
#include "IFactory.h"
#include "DisplayableImage.h"
#include "VolatileTexture.h"

#include "utils/PixelUtils.h"

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <google/protobuf/descriptor.h>
#include <sstream>
#include <string>

using namespace ::std;
using namespace ::google::protobuf;
using namespace ::duke::protocol;
using namespace ::shader_factory;


static CGprogram createProgram(IFactory& factory, const TShaderType type, const string& code, const string &name) {
    const char** pProgramOptions = factory.getShaderOptions(type);
    const CGprofile profile = factory.getShaderProfile(type);
    const CGprogram cgProgram = cgCreateProgram(factory.getCgContext(), //context
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

ShaderBasePtr compile(IFactory& factory, const ::duke::protocol::Shader& shader, const TShaderType type) {
    const string name = shader.name();
    ShaderBasePtr pShader;
    const bool isPersistent = !name.empty();

    if (isPersistent)
        resource::tryGet(factory.resourceCache, name, pShader);
    if (!pShader) {
        string code;
        if (type == SHADER_VERTEX || shader.has_code())
            code = shader.code();
        else {
            // shader must have either a shading tree or code
            assert(shader.has_program());
            code = shader_factory::compile(shader.program(), factory.prototypeFactory);
        }
        CGprogram program = createProgram(factory, type, code, name);
        pShader.reset(factory.createShader(program, type));
        if (isPersistent)
            resource::put(factory.resourceCache, name, pShader);
    }
    assert( pShader->getType() == type);
    return pShader;
}
