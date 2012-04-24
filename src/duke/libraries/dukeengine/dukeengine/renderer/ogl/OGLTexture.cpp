#include "OGLTexture.h"
#include "OGLRenderer.h"
#include "OGLEnum.h"
#include <dukeengine/renderer/utils/PixelFormatUtils.h>
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif
#include <stdexcept>
#include <cassert>

OGLTexture::OGLTexture(const ImageDescription& description, unsigned long usageFlag, OGLRenderer& renderer) :
                ITextureBase(description, renderer.getCompliantFormat(description.format), usageFlag), m_Texture(0), m_Renderer(renderer), //
                oglFormat(OGLEnum::GetFormat(getFormat())), oglType(OGLEnum::GetType(getFormat())) {

    glGenTextures(1, &m_Texture);

    if (getDepth() <= 1) {
        glBindTexture(GL_TEXTURE_2D, m_Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, OGLEnum::GetInternalFormat(getFormat()), getWidth(), getHeight(), 0, oglFormat, oglType, NULL);
    } else {
        glBindTexture(GL_TEXTURE_3D, m_Texture);
        glTexImage3D(GL_TEXTURE_3D, 0, OGLEnum::GetInternalFormat(getFormat()), getWidth(), getHeight(), getDepth(), 0, oglFormat, oglType, NULL);
    }
}

OGLTexture::~OGLTexture() {
    if (m_Texture)
        glDeleteTextures(1, &m_Texture);
}

void OGLTexture::update(const unsigned char* pData) {
    if (m_Renderer.hasCapability(CAP_PIXEL_BUFFER_OBJECT)) {

        const int imageLineSize = getWidth() * bytesPerPixel(getFormat());
        const int dataSize = imageLineSize * getHeight();

        glBindTexture(GL_TEXTURE_2D, m_Texture);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_Renderer.getPBO());
        glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, dataSize, pData, GL_STREAM_DRAW_ARB);
        if (getDepth() <= 1) {
            glBindTexture(GL_TEXTURE_2D, m_Texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, getWidth(), getHeight(), oglFormat, oglType, 0);
        } else {
            glBindTexture(GL_TEXTURE_3D, m_Texture);
            glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, getWidth(), getHeight(), getDepth(), oglFormat, oglType, 0);
        }
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    } else {
        if (getDepth() <= 1) {
            glBindTexture(GL_TEXTURE_2D, m_Texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, getWidth(), getHeight(), oglFormat, oglType, pData);
        } else {
            glBindTexture(GL_TEXTURE_3D, m_Texture);
            glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, getWidth(), getHeight(), getDepth(), oglFormat, oglType, pData);
        }
    }

#ifdef DEBUG
    const GLenum errCode = glGetError();
    if (errCode != GL_NO_ERROR) {
        std::cerr << "OGL Error: ";
        switch (errCode) {
            case GL_INVALID_OPERATION:
                std::cerr << "GL_INVALID_OPERATION" << std::endl;
                break;
            case GL_INVALID_VALUE:
                std::cerr << "GL_INVALID_VALUE" << std::endl;
                break;
            case GL_INVALID_ENUM:
                std::cerr << "GL_INVALID_ENUM" << std::endl;
                break;
            default:
                break;
        }
    }
#endif

}

