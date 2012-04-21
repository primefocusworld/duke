#ifndef IRENDERER_H_
#define IRENDERER_H_

#include "Buffer.h"
#include "Enums.h"
#include "ResourceCache.h"
#include "TexturePool.h"
#include <dukeio/ImageDescription.h>
#include <dukeapi/shading/PrototypeFactory.h>
#include <boost/utility.hpp>
#include <Cg/cg.h>

class IShaderBase;
class ITextureBase;

class IRenderer : public boost::noncopyable {
public:
    IRenderer();
    virtual ~IRenderer();

    template<class T> Buffer<T> createVertexBuffer(unsigned long size, unsigned long flags, const T* data) const;
    template<class T> Buffer<T> createIndexBuffer(unsigned long size, unsigned long flags, const T* data) const;
    CGcontext getCgContext() const;
    bool hasCapability(TCapability capability) const;
    CGprofile getShaderProfile(TShaderType Type) const;
    const char* * getShaderOptions(TShaderType Type) const;

    virtual IShaderBase* createShader(CGprogram program, TShaderType type) const = 0;
    virtual TPixelFormat getCompliantFormat(TPixelFormat format) const = 0;
    virtual ITextureBase* createTexture(const ImageDescription& description, unsigned long flags = 0) const = 0;
    virtual void setShader(IShaderBase* shader) = 0;
    virtual void setVertexBuffer(unsigned int stream, const IBufferBase* buffer, unsigned long stride) = 0;
    virtual void setIndexBuffer(const IBufferBase* buffer) = 0;
    virtual void drawPrimitives(TPrimitiveType meshType, unsigned long count) = 0;
    virtual void drawIndexedPrimitives(TPrimitiveType meshType, unsigned long count) = 0;
    virtual void setRenderState(const ::duke::protocol::Effect &renderState) const = 0;
    virtual void setTexture(const CGparameter sampler, //
                            const ::google::protobuf::RepeatedPtrField<duke::protocol::SamplerState>& samplerStates, //
                            const ITextureBase* pTextureBase) const = 0;
//    virtual Image dumpTexture(ITextureBase* pTextureBase) = 0;
    virtual void waitForBlanking() const = 0;
    virtual void presentFrame() = 0;
    virtual void beginScene(bool shouldClean, uint32_t cleanColor, ITextureBase* pRenderTarget = NULL) = 0;
    virtual void endScene() = 0;

protected:
    // buffers
    virtual IBufferBase* createVB(unsigned long size, unsigned long stride, unsigned long flags) const = 0;
    virtual IBufferBase* createIB(unsigned long size, unsigned long stride, unsigned long flags) const = 0;

    // capability map, each implementation have to fill it
    typedef std::map<TCapability, bool> TCapabilityMap;
    TCapabilityMap m_Capabilities;

    // profiles
    CGprofile m_VSProfile;
    CGprofile m_PSProfile;
    const char** m_VSOptions;
    const char** m_PSOptions;
    virtual void checkCaps() = 0;

private:
    void add(const duke::protocol::FunctionPrototype &);
    CGcontext m_Context;
public:
    resource::ResourceCache resourceCache;
    TexturePool texturePool;
    PrototypeFactory prototypeFactory;
};

template<class T>
Buffer<T> IRenderer::createVertexBuffer(unsigned long size, unsigned long flags, const T* data) const {
    Buffer<T> buffer(createVB(size, sizeof(T), flags));
    if (data)
        buffer.fill(data, size);
    return buffer;
}

template<class T>
Buffer<T> IRenderer::createIndexBuffer(unsigned long size, unsigned long flags, const T* data) const {
    Buffer<T> buffer(createIB(size, sizeof(T), flags));
    if (data)
        buffer.fill(data, size);
    return buffer;
}

#endif /* IRENDERER_H_ */
