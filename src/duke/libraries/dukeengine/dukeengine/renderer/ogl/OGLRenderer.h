#ifndef OGLRENDERER_H_
#define OGLRENDERER_H_

#include <GL/glew.h>
#include <player.pb.h>
#include <dukeengine/renderer/IRenderer.h>

class OGLRenderer : public IRenderer
{
public:
	OGLRenderer();
	~OGLRenderer();

	// IFactory
	virtual IBufferBase*  createVB( unsigned long size, unsigned long stride, unsigned long flags ) const;
	virtual IBufferBase*  createIB( unsigned long size, unsigned long stride, unsigned long flags ) const;
	virtual IShaderBase*  createShader( CGprogram program, TShaderType type ) const;
	virtual ITextureBase* createTexture( const ImageDescription& description, long unsigned int );
	virtual void          checkCaps();

	// IRenderer
	virtual void initializeGL();
	virtual void setShader( IShaderBase* shader );
	virtual void setVertexBuffer( unsigned int stream, const IBufferBase* buffer, unsigned long stride );
	virtual void setIndexBuffer( const IBufferBase* buffer );
	virtual void drawPrimitives( TPrimitiveType meshType, unsigned long count );
	virtual void drawIndexedPrimitives( TPrimitiveType meshType, unsigned long count );
    virtual void setRenderState( const ::duke::protocol::Effect &renderState ) const;
	virtual void setTexture( const CGparameter sampler, const ::google::protobuf::RepeatedPtrField< ::duke::protocol::SamplerState >& samplerStates, const ITextureBase* pTextureBase ) const;
	virtual void beginScene( bool shouldClean, uint32_t cleanColor, ITextureBase* pRenderTarget = NULL );
	virtual void endScene();
	virtual void presentFrame();
    virtual void waitForBlanking() const;
    virtual void windowResized(unsigned width, unsigned height) const;
//	virtual Image dumpTexture( ITextureBase* pTextureBase );

	TPixelFormat  getCompliantFormat(TPixelFormat format) const;
    GLuint getPBO() {
        m_lastPBOUsed = (m_lastPBOUsed + 1) % 2;
        return m_Pbo[m_lastPBOUsed];
    }

private:
	GLuint m_Fbo;
	GLuint m_RenderBuffer;
	GLuint m_Pbo[2];
	size_t m_lastPBOUsed;
};

#endif /* OGLRENDERER_H_ */
