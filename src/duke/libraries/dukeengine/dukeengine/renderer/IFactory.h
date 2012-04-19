#ifndef IFACTORY_H_
#define IFACTORY_H_

#include "Buffer.h"
#include "Enums.h"
#include "ResourceCache.h"
#include "TexturePool.h"
#include "math/Vector2.h"
#include <dukeio/ImageDescription.h>
#include <dukeapi/shading/PrototypeFactory.h>
#include <boost/utility.hpp>
#include <Cg/cg.h>

class IShaderBase;
class ITextureBase;

class IFactory : public boost::noncopyable {
public:
    IFactory();
    virtual ~IFactory();

    template<class T>
    Buffer<T> createVertexBuffer(unsigned long size, unsigned long flags, const T* data) const {
        Buffer<T> buffer(createVB(size, sizeof(T), flags));
        if (data)
            buffer.fill(data, size);
        return buffer;
    }

    template<class T>
    Buffer<T> createIndexBuffer(unsigned long size, unsigned long flags, const T* data) const {
        Buffer<T> buffer(createIB(size, sizeof(T), flags));
        if (data)
            buffer.fill(data, size);
        return buffer;
    }

    // shader prototypes
    PrototypeFactory& getPrototypeFactory(){return m_PrototypeFactory;}

    // shaders
    virtual IShaderBase* createShader(CGprogram program, TShaderType type) const = 0;

    // texture
    virtual TPixelFormat getCompliantFormat(TPixelFormat format) const = 0;
    virtual ITextureBase* createTexture(const ImageDescription& description, unsigned long flags = 0) const = 0;
    CGcontext getCgContext() const;
    bool hasCapability(TCapability capability) const;
    CGprofile getShaderProfile(TShaderType Type) const;
    const char* * getShaderOptions(TShaderType Type) const;

    // resource manager
    resource::ResourceCache & getResourceManager(){return m_ResourceManager;}
    TexturePool& getTexturePool(){return m_TexturePool;}

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
    CGcontext m_Context;
    resource::ResourceCache m_ResourceManager;
    TexturePool m_TexturePool;
    PrototypeFactory m_PrototypeFactory;

    void add(const duke::protocol::FunctionPrototype &);
};

#endif /* IFACTORY_H_ */
