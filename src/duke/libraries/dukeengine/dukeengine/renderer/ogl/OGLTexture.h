#ifndef OGLTEXTURE_H_
#define OGLTEXTURE_H_

#include <GL/glew.h>
#include <dukeengine/renderer/resource/ITextureBase.h>

class OGLRenderer;

class OGLTexture : public ITextureBase {
public:
    OGLTexture(const ImageDescription& description, unsigned long usageFlag, OGLRenderer& renderer);
    ~OGLTexture();

    inline unsigned int getGLTexture() const {
        return m_Texture;
    }

private:
    virtual void update(const unsigned char* pData);
    GLuint m_Texture;
    OGLRenderer& m_Renderer;
    const GLint oglFormat;
    const GLint oglType;
};

#endif /* OGLTEXTURE_H_ */
