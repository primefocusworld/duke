#ifndef IMESHBASE_H_
#define IMESHBASE_H_

#include "ResourceCache.h"
#include "Buffer.h"
#include "Enums.h"

class IFactory;

class IMeshBase : public ::resource::IResource {
public:
    struct TVertex {
        float x, y, z;
        float u, v;
    };
    typedef unsigned short TIndex;

    IMeshBase(TPrimitiveType type, Buffer<TVertex> vertexBuffer, Buffer<TIndex> indexBuffer);
    virtual ~IMeshBase();

    void render(IFactory&) const;

private:
    unsigned long getPrimitiveCount() const;
    TPrimitiveType m_PrimitiveType;
    Buffer<TVertex> m_VertexBuffer;
    Buffer<TIndex> m_IndexBuffer;
    unsigned long m_iPrimitiveCount;
};

#endif /* IMESHBASE_H_ */
