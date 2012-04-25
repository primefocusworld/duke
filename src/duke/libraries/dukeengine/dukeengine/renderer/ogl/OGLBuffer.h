#ifndef OGLBUFFER_H_
#define OGLBUFFER_H_

#include <GL/glew.h>
#include <dukeengine/renderer/resource/IBufferBase.h>

template<int T>
class OGLBuffer : public IBufferBase {
public:
    OGLBuffer(unsigned long count, unsigned int buffer);
    virtual ~OGLBuffer();

    unsigned int getBuffer() const;

protected:
    virtual void* lock(unsigned long offset, unsigned long size, unsigned long flags);
    virtual void unlock();

    GLuint m_pBuffer;
};

typedef OGLBuffer<GL_ARRAY_BUFFER_ARB> OGLVertexBuffer;
typedef OGLBuffer<GL_ELEMENT_ARRAY_BUFFER_ARB> OGLIndexBuffer;

#include "OGLEnum.h"

template<int T>
inline OGLBuffer<T>::OGLBuffer(unsigned long count, unsigned int buffer) :
                IBufferBase(count), m_pBuffer(buffer) {
}

template<int T>
OGLBuffer<T>::~OGLBuffer() {
    if (m_pBuffer)
        glDeleteBuffersARB(1, &m_pBuffer);
}

template<int T>
unsigned int OGLBuffer<T>::getBuffer() const {
    return m_pBuffer;
}

template<int T>
void* OGLBuffer<T>::lock(unsigned long offset, unsigned long size, unsigned long flags) {
    glBindBufferARB(T, m_pBuffer);
    unsigned char* buffer = reinterpret_cast<unsigned char*>(glMapBufferARB(T, OGLEnum::LockFlags(flags)));

    return buffer + offset;
}

template<int T>
void OGLBuffer<T>::unlock() {
    glUnmapBufferARB(T);
}

#endif /* OGLBUFFER_H_ */
