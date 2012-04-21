#ifndef IRENDERER_H_
#define IRENDERER_H_

#include "Enums.h"
#include "Mesh.h"
#include "IFactory.h"
#include "Image.h"
#include "RenderingContext.h"
#include "IShaderBase.h"

#include <player.pb.h>

#include <dukeengine/RenderInterface.h>

#include <SFML/System.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

class ITextureBase;
class IBufferBase;

class RenderingEngine {
public:
    RenderingEngine(const duke::protocol::Renderer&, sf::Window&, IRendererHost&, IFactory& factory);
    virtual ~RenderingEngine();

    void loop();

protected:
    friend struct RAIIScene;

    // executes a simulation step : implements frame logic
    bool simulationStep();

    sf::Window& m_Window;

private:
    friend class SfmlWindow;
    void waitForBlankingAndWarn(bool presented) const;
    void consumeUntilEngine();
    void displayClip(const ::duke::protocol::Clip&);
    void displayPass(const ::duke::protocol::RenderPass&);
    void displayMeshWithName(const std::string& name);
    void compileAndSetShader(const TShaderType&, const std::string&);
    friend class ShaderFactory;
    const ImageDescription& getSafeImageDescription(const ImageDescription*) const;
    const ImageDescription& getImageDescriptionFromClip(const std::string &clipName) const;
    void applyParameter(const ShaderBasePtr&, const std::string&);
    void applyParameter(const ShaderBasePtr&, const std::string &, const ::duke::protocol::AutomaticParameter&);
    void applyParameter(const ShaderBasePtr&, const std::string &, const ::duke::protocol::StaticParameter&);

    template<typename T>
    inline void addResource(const ::google::protobuf::serialize::MessageHolder&);

    inline const Setup& getSetup() const {
        return m_Host.m_Setup;
    }

    const duke::protocol::Renderer m_Configuration;
    IRendererHost& m_Host;
    IFactory& m_Factory;
    sf::Event m_Event;
    unsigned long m_DisplayedFrameCount;
    ::duke::protocol::Engine m_EngineStatus;
    ImageDescription m_EmptyImageDescription;
    bool m_bRenderOccured;
    RenderingContext m_Context;
};

#endif /* IRENDERER_H_ */
