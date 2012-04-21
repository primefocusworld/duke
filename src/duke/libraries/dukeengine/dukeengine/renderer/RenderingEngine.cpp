#include "RenderingEngine.h"
#include "VolatileTexture.h"
#include "DisplayableImage.h"
#include "ProtoBufResource.h"
#include "utils/SfmlProtobufUtils.h"
#include "ShaderFactory.h"

#include <dukeapi/ProtobufSerialize.h>

#include <google/protobuf/descriptor.h>

#include <boost/thread.hpp>
#include <boost/foreach.hpp>

#include <iostream>

using namespace ::duke::protocol;
using namespace ::google::protobuf;
using namespace ::std;

using namespace google::protobuf::serialize;

const char emptyImageData[] = { CHAR_MAX, CHAR_MIN, CHAR_MAX, CHAR_MIN };
const string HEADER = "[RenderingEngine] ";

/**
 * RAII to enforce end scene is paired with begin scene
 */
struct RAIIScene {
    RAIIScene(IRenderer& renderer, bool &renderOccured, bool shouldClean, uint32_t cleanColor, ITextureBase* pRenderTarget = NULL) :
                    m_Renderer(renderer), m_bRenderOccured(renderOccured) {
        m_Renderer.beginScene(shouldClean, cleanColor, pRenderTarget);
    }
    ~RAIIScene() {
        m_Renderer.endScene();
        m_bRenderOccured = true;
    }
private:
    IRenderer &m_Renderer;
    bool &m_bRenderOccured;
};

/**
 * RAII to enforce context is popped if pushed
 */
struct RAIIContext {
    RAIIContext(RenderingContext& context, const string& name, const bool doPush) :
                    m_Scopes(context.scopes), m_doPush(doPush) {
        if (m_doPush)
            m_Scopes.push_back(name);
    }
    ~RAIIContext() {
        if (m_doPush)
            m_Scopes.pop_back();
    }
private:
    Scopes &m_Scopes;
    const bool m_doPush;
};

/**
 * templated helper to add a protobuf resource in the resource manager
 */
template<typename T>
inline void RenderingEngine::addResource(const ::google::protobuf::serialize::MessageHolder& holder) {
    const T msg = unpackTo<T>(holder);
    resource::put(m_Factory.resourceCache, msg.name(), boost::shared_ptr<ProtoBufResource>(new ProtoBufResource(msg)));
}

RenderingEngine::RenderingEngine(const duke::protocol::Renderer& renderer, sf::Window& window, IRendererHost& host, IRenderer& factory) :
                m_Window(window), m_Configuration(renderer), m_Host(host), m_Factory(factory), m_DisplayedFrameCount(0), m_bRenderOccured(false) {
    m_EmptyImageDescription.width = 1;
    m_EmptyImageDescription.height = 1;
    m_EmptyImageDescription.depth = 0;
    m_EmptyImageDescription.pImageData = emptyImageData;
    m_EmptyImageDescription.imageDataSize = sizeof(emptyImageData);
    m_EmptyImageDescription.format = PXF_R8G8B8A8;
    m_EngineStatus.set_action(Engine::RENDER_START);
}

void RenderingEngine::loop() {
    bool bLastFrame = false;
    while (!bLastFrame) {
        // calling a simulation step
        try {
            bLastFrame = simulationStep();
        } catch (exception& e) {
            cerr << HEADER + "Unexpected error : " + e.what() << endl;
            boost::this_thread::sleep(boost::posix_time::millisec(200));
        } catch (...) {
            cerr << HEADER + "Unexpected error." << endl;
        }
        ++m_DisplayedFrameCount;
    }
}

static void dump(const google::protobuf::Descriptor* pDescriptor, const google::protobuf::serialize::MessageHolder &holder) {
#ifdef DEBUG_MESSAGES
    cerr << HEADER + "pop " + pDescriptor->name() << "\t" << unpack(holder)->ShortDebugString() << endl;
#endif
}

void RenderingEngine::consumeUntilEngine() {
    // updating resources by popping all the pending messages
    const MessageHolder* pHolder;
    while ((pHolder = m_Host.popEvent()) != NULL) {
        const MessageHolder &holder(*pHolder);
        const Descriptor *pDescriptor(descriptorFor(holder));
        holder.CheckInitialized();
        dump(pDescriptor, holder);

        if (isType<Shader>(pDescriptor)) {
            addResource<Shader>(holder);
        } else if (isType<duke::protocol::Mesh>(pDescriptor)) {
            addResource<duke::protocol::Mesh>(holder);
        } else if (isType<Texture>(pDescriptor)) {
            const Texture texture = unpackTo<Texture>(holder);
            switch (holder.action()) {
                case MessageHolder::CREATE:
                case MessageHolder::UPDATE:
                    DisplayableImage(m_Factory, texture);
                    break;
                default: {
                    ostringstream msg;
                    msg << HEADER << "IRenderer : don't know how to action " << MessageHolder_Action_Name(holder.action()) << " on a Texture" << endl;
                    throw runtime_error(msg.str());
                }
            }
        } else if (isType<StaticParameter>(pDescriptor)) {
            addResource<StaticParameter>(holder);
        } else if (isType<AutomaticParameter>(pDescriptor)) {
            addResource<AutomaticParameter>(holder);
        } else if (isType<Grading>(pDescriptor)) {
            addResource<Grading>(holder);
        } else if (isType<Event>(pDescriptor)) {
            const Event event = unpackTo<Event>(holder);
            if (event.type() == Event::RESIZED) {
                const ResizeEvent &resizeEvent = event.resizeevent();
                if (resizeEvent.has_height() && resizeEvent.has_width())
                    m_Window.SetSize(resizeEvent.width(), resizeEvent.height());
                if (resizeEvent.has_x() && resizeEvent.has_y())
                    m_Window.SetPosition(resizeEvent.x(), resizeEvent.y());
            }
        } else if (isType<FunctionPrototype>(pDescriptor)) {
            m_Factory.prototypeFactory.setPrototype(unpackTo<FunctionPrototype>(holder));
        } else if (isType<Engine>(pDescriptor)) {
            m_EngineStatus.CopyFrom(unpackTo<Engine>(holder));
            if (m_EngineStatus.action() != Engine::RENDER_STOP)
                return;
        } else {
            ostringstream msg;
            msg << HEADER << "IRenderer : unknown message type " << pDescriptor->name() << endl;
            throw runtime_error(msg.str());
        }
    }
}

bool RenderingEngine::simulationStep() {
    m_Context.reset();
    m_Host.renderStart();

    try {
        consumeUntilEngine();
    } catch (exception& e) {
        cerr << HEADER + "Unexpected error while consuming messages in the rendering thread : " + e.what() << endl;
        return false;
    }

    // rendering clips
    {
        const bool renderRequested = m_EngineStatus.action() != Engine::RENDER_STOP;
        const bool renderAvailable = renderRequested && m_Window.IsOpened();

        if (renderAvailable) {
            try {
                m_bRenderOccured = false;
                for_each(getSetup().m_Clips.begin(), getSetup().m_Clips.end(), boost::bind(&RenderingEngine::displayClip, this, _1));
                if (!m_bRenderOccured) {
                    ::boost::this_thread::sleep(::boost::posix_time::milliseconds(10));
                } else {
                    IRendererHost::PresentStatus status;
                    while ((status = m_Host.getPresentStatus()) == IRendererHost::SKIP_NEXT_BLANKING)
                        waitForBlankingAndWarn(false);
                    m_Window.Display();
                    if (status == IRendererHost::PRESENT_NEXT_BLANKING)
                        waitForBlankingAndWarn(true);
                }
            } catch (exception& e) {
                cerr << HEADER + "Unexpected error while rendering : " + e.what() << endl;
                return false;
            }
        }

        if (m_EngineStatus.action() == Engine::RENDER_ONE)
            m_EngineStatus.set_action(Engine::RENDER_STOP);
    }

    // Sending back messages if needed
    MessageHolder holder;
    try {
        Event event;
        while (m_Window.PollEvent(m_Event)) {
            event.Clear();
            // transcoding the event to protocol buffer
            Update(event, m_Event);
            pack(event, holder);
            m_Host.pushEvent(holder);
        }
    } catch (exception& e) {
        cerr << HEADER + "Unexpected error while dispatching events : " + e.what() << endl;
        return false;
    }

    BOOST_FOREACH ( const DumpedImages::value_type &pair, m_Context.dumpedImages) {
        const string &name = pair.first;
        Texture texture;
        pair.second->dump(texture);
        texture.set_name(name);
        pack(texture, holder);
        m_Host.pushEvent(holder);
    }
    return m_Host.renderFinished(0);
}

void RenderingEngine::waitForBlankingAndWarn(bool presented) const {
    m_Factory.waitForBlanking();
    m_Host.verticalBlanking(presented);
}

void RenderingEngine::displayClip(const ::duke::protocol::Clip& clip) {
    try {
        if (!clip.has_grade() && !clip.has_gradename()) {
            cerr << HEADER + "no grading associated with clip" << endl;
            return;
        }
        if (clip.has_grade() && clip.has_gradename())
            cerr << HEADER + "clip has both grade and gradeName set : picking grade" << endl;

        // allocating context
        m_Context.set(getSetup().m_Images, m_DisplayedFrameCount, m_Window.GetWidth(), m_Window.GetHeight());
        RAIIContext clipContext(m_Context, clip.name(), clip.has_name());

        TResourcePtr pResource;
        const ::duke::protocol::Grading * pGrading = NULL;
        if (clip.has_grade()) {
            pGrading = &clip.grade();
        } else {
            pGrading = &resource::getPB<Grading>(m_Factory.resourceCache, clip.gradename());
        }
        RAIIContext gradingContext(m_Context, pGrading->name(), pGrading->has_name());
        for_each(pGrading->pass().begin(), pGrading->pass().end(), boost::bind(&RenderingEngine::displayPass, this, _1));
    } catch (exception &ex) {
        cerr << HEADER + ex.what() << " occurred while displaying clip " << clip.DebugString() << endl;
    }
}

static void overrideClipDimension(ImageDescription &description, const Texture &texture) {
    const bool hasWidth = texture.has_width();
    const bool hasHeight = texture.has_height();
    const bool hasNone = !hasWidth && !hasHeight;
    if (hasNone)
        return;
    const bool hasBoth = hasWidth && hasHeight;
    if (hasBoth) {
        description.width = texture.width();
        description.height = texture.height();
    } else {
        size_t newWidth;
        size_t newHeight;
        if (hasWidth) {
            newWidth = texture.width();
            newHeight = newWidth * description.height / description.width;
        } else {
            newHeight = texture.height();
            newWidth = newHeight * description.width / description.height;
        }
        description.width = newWidth;
        description.height = newHeight;
    }
}

void RenderingEngine::displayPass(const ::duke::protocol::RenderPass& pass) {
    try {
        // fetching the effect
        RAIIContext passContext(m_Context, pass.name(), pass.has_name());

        // preparing render target
        TexturePtr pRenderTarget;
        ITextureBase *pRenderTargetTexture = NULL;
        if (pass.target() == RenderPass::TEXTURETARG) {
            assert(pass.has_rendertargetname());
            const string &renderTargetName = pass.rendertargetname();
            RenderTargets::const_iterator itr = m_Context.renderTargets.find(renderTargetName);
            if (itr != m_Context.renderTargets.end()) {
                pRenderTarget = itr->second;
            } else {
                assert(pass.has_rendertargetdimfromclipname());
                const string& clipName = pass.rendertargetdimfromclipname();
                ImageDescription clipDescription = getImageDescriptionFromClip(clipName);
                if (pass.has_rendertarget())
                    overrideClipDimension(clipDescription, pass.rendertarget());
                pRenderTarget.reset(new VolatileTexture(m_Factory, clipDescription, TEX_RENTERTARGET));
                m_Context.renderTargets[renderTargetName] = pRenderTarget;
            }
            assert(pRenderTarget);
            pRenderTargetTexture = pRenderTarget->getTexture();
            //            cout << "rendering to " << renderTargetName << " : " << pRenderTargetTexture << endl;
        }

        // setting render target dimensions
        if (pRenderTargetTexture)
            m_Context.setRenderTarget(pRenderTargetTexture->getWidth(), pRenderTargetTexture->getHeight());
        else
            m_Context.setRenderTarget(m_Window.GetWidth(), m_Window.GetHeight());

        {
            RAIIScene scenePass(m_Factory, m_bRenderOccured, pass.clean(), pass.cleancolor(), pRenderTargetTexture);
            if (pass.has_effect() && pass.meshname_size() != 0) {
                const Effect& effect = pass.effect();
                // setting render state
                m_Factory.setRenderState(effect);

                // setting shaders
                assert(effect.has_vertexshadername());
                compileAndSetShader(SHADER_VERTEX, effect.vertexshadername());
                assert(effect.has_pixelshadername());
                compileAndSetShader(SHADER_PIXEL, effect.pixelshadername());

                // render all meshes
                for_each(pass.meshname().begin(), pass.meshname().end(), boost::bind(&RenderingEngine::displayMeshWithName, this, _1));
            }
        }

        // dumpTexture
        // TODO get texture grabbing back
//        if (pass.has_grab()) {
//            DumpedImages &images = m_Context.dumpedImages;
//            if (pRenderTargetTexture) {
//                //images[pass.grab().name()].reset(new Image(dumpTexture(pRenderTargetTexture)));
//            } else {
//                cerr << "A grab is requested " << pass.name() << ", but render target is NULL" << endl;
//            }
//        }

    } catch (exception &ex) {
        cerr << HEADER + ex.what() << " occurred while displaying pass " << pass.DebugString() << endl;
    }
}

void RenderingEngine::displayMeshWithName(const std::string& name) {
    const ::duke::protocol::Mesh& mesh = resource::getPB<duke::protocol::Mesh>(m_Factory.resourceCache, name);
    ::Mesh(m_Factory, mesh).render(m_Factory);
}

void RenderingEngine::compileAndSetShader(const TShaderType& type, const string& name) {
    const Shader &shader = resource::getPB<Shader>(m_Factory.resourceCache, name);
    ShaderBasePtr pShader = compile(m_Factory, shader, type);
    assert(pShader);

    const vector<string> &params = pShader->getParameterNames();
    for_each(params.begin(), params.end(), //
             boost::bind(&RenderingEngine::applyParameter, this, boost::ref(pShader), _1));

    m_Factory.setShader(pShader.get());
}

static ProtoBufResource& getParam(resource::ResourceCache& cache, const RenderingContext &context, const string &name) {
    for (ScopesRItr it = context.scopes.rbegin(); it < context.scopes.rend(); ++it) {
        const string scopedParamName = *it + '|' + name;
        TResourcePtr pParam;
        resource::tryGet(cache, scopedParamName, pParam);
        if (pParam != NULL)
            return *pParam;
    }
    return resource::get<ProtoBufResource>(cache, name);
}

void RenderingEngine::applyParameter(const ShaderBasePtr &pShader, const string &paramName) {
    const ProtoBufResource &param = getParam(m_Factory.resourceCache, m_Context, paramName);
    const Descriptor* pDescriptor = param.getRef<Message>().GetDescriptor();

    if (pDescriptor == StaticParameter::descriptor())
        applyParameter(pShader, paramName, param.getRef<StaticParameter>());
    else if (pDescriptor == AutomaticParameter::descriptor())
        applyParameter(pShader, paramName, param.getRef<AutomaticParameter>());
    else {
        cerr << "got unknown parameter type named : " << endl;
        param.getRef<Message>().PrintDebugString();
    }
}
void RenderingEngine::applyParameter(const ShaderBasePtr &pShader, const string& paramName, const AutomaticParameter& param) {
    float data[3] = { 1.f, 1.f, 1.f };
    switch (param.type()) {
        case AutomaticParameter::FLOAT3_TEX_DIM: {
            ImageDescription imageDescriptionHolder;
            const ImageDescription *pImageDescription = NULL;
            if (param.has_samplingsource()) {
                const SamplingSource &samplingSource = param.samplingsource();
                assert(samplingSource.has_name());
                assert(samplingSource.has_type());
                const string &sourceName = samplingSource.name();
                switch (samplingSource.type()) {
                    case SamplingSource::CLIP: {
                        pImageDescription = &getImageDescriptionFromClip(sourceName);
                        break;
                    }
                    case SamplingSource::SUPPLIED: {
                        const DisplayableImage image(m_Factory, sourceName);
                        imageDescriptionHolder = image.getImageDescription();
                        pImageDescription = &imageDescriptionHolder;
                        break;
                    }
                    case SamplingSource::SURFACE: {
                        RenderTargets::const_iterator itr = m_Context.renderTargets.find(sourceName);
                        if (itr != m_Context.renderTargets.end())
                            pImageDescription = &(itr->second->getImageDescription());
                        break;
                    }
                    default:
                        assert(!"not yet implemented");
                }
            }
            data[0] = pImageDescription ? pImageDescription->width : m_Context.renderTargetWidth();
            data[1] = pImageDescription ? pImageDescription->height : m_Context.renderTargetHeight();
            data[2] = data[0] / data[1];
//            cout << "setting " << paramName << " to " << data[0] << " " << data[1] << " " << data[2] << endl;
            break;
        }
        case AutomaticParameter::FLOAT3_TIME: {
            data[0] = m_Context.displayedFrameCount();
            data[1] = 0;
            data[2] = 0;
            break;
        }
        default: {
            assert(!"not yet implemented");
            break;
        }
    }
    pShader->setParameter(paramName, data, sizeof(data) / sizeof(float));
}

void RenderingEngine::applyParameter(const ShaderBasePtr &pShader, const string& paramName, const StaticParameter& param) {
    switch (param.type()) {
        case StaticParameter::FLOAT:
            pShader->setParameter(paramName, param.floatvalue().data(), param.floatvalue_size());
            break;
        case StaticParameter::SAMPLER: {
            assert(param.has_samplingsource());
            const SamplingSource &samplingSource = param.samplingsource();
            const string &sourceName = samplingSource.name();
            TexturePtr pTexture;
//            ostringstream debugString;
//            debugString << "setting sampler " << paramName;
            switch (samplingSource.type()) {
                case SamplingSource::CLIP:
                    pTexture.reset(new VolatileTexture(m_Factory, getImageDescriptionFromClip(sourceName)));
//                    debugString << " from clip texture";
                    break;
                case SamplingSource::SUPPLIED:
                    pTexture.reset(new DisplayableImage(m_Factory, sourceName));
//                    debugString << " from supplied texture";
                    break;
                case SamplingSource::SURFACE: {
                    const RenderTargets &targets = m_Context.renderTargets;
                    const RenderTargets::const_iterator itr = targets.find(sourceName);
                    if (itr == targets.end())
                        throw runtime_error("unknown render target '" + sourceName + "' to sample from");
                    pTexture = itr->second;
//                    debugString << " from surface";
                    break;
                }
                default:
                    cerr << "SamplingSource with type " << SamplingSource_Type_Name(samplingSource.type()) << " is not supported" << endl;
            }
//            debugString << " '" << sourceName << "'" << endl;
//            cout << debugString.str();
            assert( pTexture);
            m_Factory.setTexture(pShader->getParameter(paramName), param.samplerstate(), pTexture->getTexture());
            m_Context.textures.push_back(pTexture);
            break;
        }
        default: {
            assert(!"not yet implemented");
            break;
        }
    }
}

inline const ImageDescription& RenderingEngine::getSafeImageDescription(const ImageDescription* pImage) const {
    return (pImage == NULL || pImage->blank()) ? m_EmptyImageDescription : *pImage;
}

const ImageDescription& RenderingEngine::getImageDescriptionFromClip(const string &clipName) const {
    const vector<ImageDescription> &images = getSetup().m_Images;
    if (clipName.empty())
        return getSafeImageDescription(images.empty() ? NULL : &images[0]);

    size_t index = 0;
    BOOST_FOREACH(const duke::protocol::Clip &clip ,getSetup().m_Clips) {
        if (clip.has_name() && clip.name() == clipName)
            return getSafeImageDescription(&images[index]);
        ++index;
    }

    cerr << HEADER + "no clip associated to " << clipName << endl;
    return m_EmptyImageDescription;
}
