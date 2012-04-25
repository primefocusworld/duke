#ifndef RENDERING_ENGINE_H_
#define RENDERING_ENGINE_H_

#include "Enums.h"
#include "IRenderer.h"
#include "RenderingContext.h"
#include "resource/IShaderBase.h"

#include <player.pb.h>

#include <dukeengine/RenderInterface.h>

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

class RenderingEngine {
public:
    RenderingEngine(const duke::protocol::Renderer&, sf::Window&, IRendererHost&, IRenderer& factory);
    void loop();
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

    sf::Window &m_Window;
    const duke::protocol::Renderer m_Configuration;
    IRendererHost &m_Host;
    IRenderer &m_Renderer;
    resource::ResourceCache &m_Cache;
    sf::Event m_Event;
    unsigned long m_DisplayedFrameCount;
    ::duke::protocol::Engine m_EngineStatus;
    ImageDescription m_EmptyImageDescription;
    bool m_bRenderOccured;
    RenderingContext m_Context;
    TexturePool m_TexturePool;
};

#endif /* RENDERING_ENGINE_H_ */
