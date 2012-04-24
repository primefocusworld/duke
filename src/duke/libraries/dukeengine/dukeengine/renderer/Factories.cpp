#include "Factories.h"
#include "resource/IImageBase.h"
#include "IRenderer.h"

#include "utils/ProtobufUtils.h"

#include <player.pb.h>
#include <dukeapi/shading/ShadingGraphCodeBuilder.h>

#include <fstream>
#include <sstream>

using namespace std;

ImageDescription getImageDescriptionFrom(const ::duke::protocol::Texture& texture) {
    ImageDescription description;

    description.width = texture.width();
    description.height = texture.height();
    description.depth = texture.depth();
    if (texture.has_data()) {
        description.pImageData = texture.data().c_str();
        description.imageDataSize = texture.data().size();
    }
    description.format = Enums::Get(texture.format());
    return description;
}

void putImage(resource::ResourceCache& cache, const ::duke::protocol::Texture& texture) {
    ImagePtr pImage(new IImageBase(getImageDescriptionFrom(texture)));
    cache.put(texture.name(), pImage);
}

ITextureBase* buildTexture(IRenderer& renderer, const IImageBase* pImage) {
    assert(pImage);
    return renderer.createTexture(pImage->getImageDescription(), 0);
}

TexturePtr getNamedTexture(IRenderer& renderer, const std::string& name) {
    TexturePtr pTexture;
    resource::ResourceCache &cache = renderer.resourceCache;
    if (!cache.tryGet<ITextureBase>(name, pTexture)) {
        const IImageBase &image = cache.get<IImageBase>(name);
        pTexture.reset(buildTexture(renderer, &image));
        pTexture->update(image.pixels().data());
        assert(pTexture);
        cache.put(name, pTexture);
    }
    return pTexture;
}

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

IMeshBase* buildMesh(IRenderer& renderer, const ::duke::protocol::Mesh& mesh) {
    const ::duke::protocol::VertexBuffer& vb = mesh.vertexbuffer();
    std::vector<IMeshBase::TVertex> vertices;
    for (int vertexIndex = 0; vertexIndex < vb.vertex_size(); vertexIndex++) {
        const ::duke::protocol::Vertex& vertex = vb.vertex(vertexIndex);
        IMeshBase::TVertex vertexStruct;
        vertexStruct.x = vertex.data(0);
        vertexStruct.y = vertex.data(1);
        vertexStruct.z = vertex.data(2);
        vertexStruct.u = vertex.data(3);
        vertexStruct.v = vertex.data(4);
        vertices.push_back(vertexStruct);
    }
    Buffer<IMeshBase::TVertex> vertexBuffer(renderer.createVertexBuffer(vertices.size(), BUF_STATIC, vertices.data()));
    if (mesh.has_indexbuffer()) {
        const ::duke::protocol::IndexBuffer& ib = mesh.indexbuffer();
        const int indicesCount = ib.index_size();
        std::vector<IMeshBase::TIndex> indices;
        indices.reserve(indicesCount);
        for (int i = 0; i < indicesCount; i++)
            indices.push_back(ib.index(i));
        Buffer<IMeshBase::TIndex> indexBuffer(renderer.createIndexBuffer(indicesCount, BUF_STATIC, indices.data()));
        return new IMeshBase(Enums::Get(mesh.type()), vertexBuffer, indexBuffer);
    } else
        return new IMeshBase(Enums::Get(mesh.type()), vertexBuffer, NULL);
}

void dump(const IImageBase* pImage, const string& filename) {
    if (!pImage)
        return;

    const ImageDescription &description(pImage->getImageDescription());
    const size_t elements = description.width * description.height;

    vector<unsigned char> data;
    data.reserve(elements * 3 * sizeof(char));
    const unsigned char* pSrcData = pImage->pixels().data();
    for (size_t i = 0; i < elements; ++i) {
        data.push_back(pSrcData[2]);
        data.push_back(pSrcData[1]);
        data.push_back(pSrcData[0]);
        pSrcData += 4;
    }

    ostringstream headerStream;
    headerStream << "P6" << endl;
    headerStream << description.width << " " << description.height << endl;
    headerStream << "255" << endl;
    const string header = headerStream.str();

    //    const HANDLE file = CreateFile(filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING, NULL);
    //    DWORD read;
    //    WriteFile(file, header.data(), header.size(), &read, NULL);
    //    WriteFile(file, data.data(), data.size(), &read, NULL);
    //    CloseHandle(file);
    ofstream file(filename.c_str(), ios_base::binary | ios_base::out | ios_base::trunc);
    file.write(header.data(), header.size());
    file.write(reinterpret_cast<char*>(data.data()), data.size());
    file.close();
}

using namespace duke::protocol;

void dump(const IImageBase* pImage, Texture &texture) {
    if (!pImage)
        return;
    texture.Clear();

    const ImageDescription &description(pImage->getImageDescription());
    texture.set_width(description.width);
    texture.set_height(description.height);
    texture.set_depth(description.depth);
    texture.set_format(Enums::Get(description.format));
    texture.set_data(pImage->pixels().data(), pImage->pixels().size());
}
