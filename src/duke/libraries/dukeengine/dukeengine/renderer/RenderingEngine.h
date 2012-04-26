#ifndef RENDERING_ENGINE_H_
#define RENDERING_ENGINE_H_

#include "Enums.h"
#include "IRenderer.h"
#include "RenderingContext.h"
#include "resource/IShaderBase.h"
#include "ogl/OGLRenderer.h"

#include <player.pb.h>

#include <dukeengine/RenderInterface.h>

#include <QGLWidget>
#include <QTimer>

class RenderingEngine : public QGLWidget {
    Q_OBJECT
public:
    RenderingEngine(IRendererHost&, const QGLFormat& format, QWidget* parent = 0, const QGLWidget* shareWidget = 0, Qt::WindowFlags f = 0);
    virtual ~RenderingEngine();
protected:
    virtual void initializeGL();
    virtual void resizeGL(int width, int height);
    virtual bool event ( QEvent * e );
protected slots:
    virtual void updateGL();
private:
    bool simulationStep();
    void waitForBlankingAndWarn(bool presented) const;
    void consumeUntilEngine();
    void displayClip(const ::duke::protocol::Clip&);
    void displayPass(const ::duke::protocol::RenderPass&);
    void displayMeshWithName(const std::string& name);
    void compileAndSetShader(const TShaderType&, const std::string&);
    const ImageDescription& getSafeImageDescription(const ImageDescription*) const;
    const ImageDescription& getImageDescriptionFromClip(const std::string &clipName) const;
    void applyParameter(const ShaderPtr&, const std::string&);
    void applyParameter(const ShaderPtr&, const std::string &, const ::duke::protocol::AutomaticParameter&);
    void applyParameter(const ShaderPtr&, const std::string &, const ::duke::protocol::StaticParameter&);

    template<typename T>
    inline void addResource(const ::google::protobuf::serialize::MessageHolder&);

    inline const Setup& getSetup() const {
        return m_Host.m_Setup;
    }

    IRendererHost &m_Host;
    OGLRenderer m_Renderer;
    resource::ResourceCache &m_Cache;
    unsigned long m_DisplayedFrameCount;
    ::duke::protocol::Engine m_EngineStatus;
    ImageDescription m_EmptyImageDescription;
    bool m_bRenderOccured;
    RenderingContext m_Context;
    TexturePool m_TexturePool;
    QTimer m_RefreshTimer;
};

#endif /* RENDERING_ENGINE_H_ */
