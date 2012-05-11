/*
 * DukeEngine_Render.cpp
 *
 *  Created on: May 7, 2012
 *      Author: Guillaume Chatelet
 */

#include "DukeEngine.h"

#include "resource/ProtoBufResource.h"
#include "DukeWidget.h"
#include "Factories.h"
#include "RenderingContext.h"

#include <google/protobuf/descriptor.h>

using namespace ::duke::protocol;
using namespace ::google::protobuf;
using namespace ::google::protobuf::serialize;

static const string HEADER = "[DukeEngine] ";
static const char emptyImageData[] = { CHAR_MAX, CHAR_MIN, CHAR_MAX, CHAR_MIN };

static ImageDescription buildEmpty() {
    ImageDescription desc;
    desc.width = 1;
    desc.height = 1;
    desc.depth = 0;
    desc.pImageData = emptyImageData;
    desc.imageDataSize = sizeof(emptyImageData);
    desc.format = PXF_R8G8B8A8;
    return desc;
}

static const ImageDescription m_EmptyImageDescription = buildEmpty();

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
struct RenderHelper {
    RenderHelper(DukeEngine &engine, DukeWidget &widget) :
                    engine(engine), widget(widget), m_Renderer(widget.renderer), m_TexturePool(widget.texturePool), m_Cache(widget.renderer.resourceCache), m_EngineStatus(
                                    engine.m_EngineStatus), m_RenderQueue(engine.m_RenderQueue), m_Images(engine.m_Images), m_Clips(engine.m_Clips), m_bRenderOccured(false) {
    }

    void consumeRenderMessages();
    void render();

private:
    template<typename T>
    void addResource(const ::google::protobuf::serialize::MessageHolder& holder) {
        const T msg = unpackTo<T>(holder);
        m_Cache.put(msg.name(), boost::shared_ptr<ProtoBufResource>(new ProtoBufResource(msg)));
    }

    void displayClip(const ::duke::protocol::Clip&);
    void displayPass(const ::duke::protocol::RenderPass&);
    void displayMeshWithName(const string&);
    void compileAndSetShader(const TShaderType&, const string&);
    void applyParameter(const ShaderPtr&, const std::string&);
    void applyParameter(const ShaderPtr&, const std::string &, const ::duke::protocol::AutomaticParameter&);
    void applyParameter(const ShaderPtr&, const std::string &, const ::duke::protocol::StaticParameter&);
    const ImageDescription& getSafeImageDescription(const ImageDescription*) const;
    const ImageDescription& getImageDescriptionFromClip(const std::string &clipName) const;

    DukeEngine &engine;
    DukeWidget &widget;
    IRenderer &m_Renderer;
    TexturePool &m_TexturePool;
    resource::ResourceCache &m_Cache;
    Engine &m_EngineStatus;
    MessageQueue &m_RenderQueue;
    const std::vector<ImageDescription> &m_Images;
    const std::vector<duke::protocol::Clip> &m_Clips;
    RenderingContext m_Context;
    bool m_bRenderOccured;
};

static void dump(const google::protobuf::Descriptor* pDescriptor, const google::protobuf::serialize::MessageHolder &holder) {
#ifdef DEBUG_MESSAGES
    cerr << HEADER + "pop " + pDescriptor->name() << "\t" << unpack(holder)->ShortDebugString() << endl;
#endif
}

void RenderHelper::consumeRenderMessages() {
    // updating resources by popping all the pending messages
    SharedHolder pHolder;
    while (m_RenderQueue.tryPop(pHolder)) {
        const MessageHolder &holder(*pHolder);
        const Descriptor *pDescriptor(descriptorFor(holder));
        holder.CheckInitialized();
        dump(pDescriptor, holder);

        if (isType<Shader>(pDescriptor)) {
            addResource<Shader>(holder);
        } else if (isType<Mesh>(pDescriptor)) {
            addResource<Mesh>(holder);
        } else if (isType<Texture>(pDescriptor)) {
            const Texture texture = unpackTo<Texture>(holder);
            switch (holder.action()) {
                case MessageHolder::CREATE:
                case MessageHolder::UPDATE:
                    putImage(m_Cache, texture);
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
                if (resizeEvent.has_height() && resizeEvent.has_width()) {
                    widget.resize(resizeEvent.width(), resizeEvent.height());
                    widget.renderer.windowResized(resizeEvent.width(), resizeEvent.height());
                }
                if (resizeEvent.has_x() && resizeEvent.has_y())
                    widget.move(resizeEvent.x(), resizeEvent.y());
            }
        } else if (isType<FunctionPrototype>(pDescriptor)) {
            widget.renderer.prototypeFactory.setPrototype(unpackTo<FunctionPrototype>(holder));
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
void RenderHelper::render() {
    const bool renderRequested = m_EngineStatus.action() != Engine::RENDER_STOP;
    const bool renderAvailable = renderRequested && widget.isValid();

    if (renderAvailable) {
        m_bRenderOccured = false;
        for_each(m_Clips.begin(), m_Clips.end(), bind(&RenderHelper::displayClip, this, _1));
        if (!m_bRenderOccured) {
            ::boost::this_thread::sleep(::boost::posix_time::milliseconds(10));
        } else {
            DukeEngine::PresentStatus status;
            while ((status = engine.getPresentStatus()) == DukeEngine::SKIP_NEXT_BLANKING)
                engine.waitForBlankingAndWarn(widget.renderer, false);
            widget.swapBuffers();
            if (status == DukeEngine::PRESENT_NEXT_BLANKING)
                engine.waitForBlankingAndWarn(widget.renderer, true);
        }
    }

    if (m_EngineStatus.action() == Engine::RENDER_ONE)
        m_EngineStatus.set_action(Engine::RENDER_STOP);
}

void RenderHelper::displayClip(const ::duke::protocol::Clip& clip) {
    try {
        if (!clip.has_grade() && !clip.has_gradename()) {
            cerr << HEADER + "no grading associated with clip" << endl;
            return;
        }
        if (clip.has_grade() && clip.has_gradename())
            cerr << HEADER + "clip has both grade and gradeName set : picking grade" << endl;

        // allocating context
        m_Context.set(m_Images, engine.m_DisplayedFrameCount, widget.width(), widget.height());
        RAIIContext clipContext(m_Context, clip.name(), clip.has_name());

        const ::duke::protocol::Grading &grading = clip.has_grade() ? clip.grade() : resource::getPB<Grading>(m_Cache, clip.gradename());
        RAIIContext gradingContext(m_Context, grading.name(), grading.has_name());
        for_each(grading.pass().begin(), grading.pass().end(), bind(&RenderHelper::displayPass, this, _1));
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

void RenderHelper::displayPass(const ::duke::protocol::RenderPass& pass) {
    try {
        // fetching the effect
        RAIIContext passContext(m_Context, pass.name(), pass.has_name());

        // preparing render target
        ITextureBase *pRenderTargetTexture = NULL;
        if (pass.target() == RenderPass::TEXTURETARG) {
            assert(pass.has_rendertargetname());
            const string &renderTargetName = pass.rendertargetname();
            RenderTargets::const_iterator itr = m_Context.renderTargets.find(renderTargetName);
            if (itr != m_Context.renderTargets.end()) {
                pRenderTargetTexture = itr->second.getTexture();
            } else {
                assert(pass.has_rendertargetdimfromclipname());
                const string& clipName = pass.rendertargetdimfromclipname();
                ImageDescription clipDescription = getImageDescriptionFromClip(clipName);
                if (pass.has_rendertarget())
                    overrideClipDimension(clipDescription, pass.rendertarget());

                VolatileTexture volatileTexture(m_TexturePool.get(clipDescription, TEX_RENTERTARGET));
                pRenderTargetTexture = volatileTexture.getTexture();
                m_Context.renderTargets[renderTargetName] = volatileTexture;
            }
            assert(pRenderTargetTexture);
        }

        // setting render target dimensions
        if (pRenderTargetTexture)
            m_Context.setRenderTarget(pRenderTargetTexture->getWidth(), pRenderTargetTexture->getHeight());
        else
            m_Context.setRenderTarget(widget.width(), widget.height());

        {
            RAIIScene scenePass(m_Renderer, m_bRenderOccured, pass.clean(), pass.cleancolor(), pRenderTargetTexture);
            if (pass.has_effect() && pass.meshname_size() != 0) {
                const Effect& effect = pass.effect();
                // setting render state
                m_Renderer.setRenderState(effect);

                // setting shaders
                assert(effect.has_vertexshadername());
                compileAndSetShader(SHADER_VERTEX, effect.vertexshadername());
                assert(effect.has_pixelshadername());
                compileAndSetShader(SHADER_PIXEL, effect.pixelshadername());

                // render all meshes
                for_each(pass.meshname().begin(), pass.meshname().end(), bind(&RenderHelper::displayMeshWithName, this, _1));
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

void RenderHelper::displayMeshWithName(const string& name) {
    getNamedMesh(m_Renderer, name)->render(m_Renderer);
}

void RenderHelper::compileAndSetShader(const TShaderType& type, const string& name) {
    ShaderPtr pShader = getNamedShader(m_Renderer, name, type);
    assert(pShader);

    const vector<string> &params = pShader->getParameterNames();
    for_each(params.begin(), params.end(), //
             bind(&RenderHelper::applyParameter, this, ref(pShader), _1));

    m_Renderer.setShader(pShader.get());
}

static ProtoBufResource& getParam(resource::ResourceCache& cache, const RenderingContext &context, const string &name) {
    for (ScopesRItr it = context.scopes.rbegin(); it < context.scopes.rend(); ++it) {
        const string scopedParamName = *it + '|' + name;
        TResourcePtr pParam;
        cache.tryGet(scopedParamName, pParam);
        if (pParam != NULL)
            return *pParam;
    }
    return cache.get<ProtoBufResource>(name);
}

void RenderHelper::applyParameter(const ShaderPtr &pShader, const string &paramName) {
    const ProtoBufResource &param = getParam(m_Cache, m_Context, paramName);
    const Descriptor* pDescriptor = param.getRef<Message>().GetDescriptor();

    if (pDescriptor == StaticParameter::descriptor())
        applyParameter(pShader, paramName, param.getRef<StaticParameter>());
    else if (pDescriptor == AutomaticParameter::descriptor())
        applyParameter(pShader, paramName, param.getRef<AutomaticParameter>());
    else
        cerr << "got unknown parameter type named : " << param.getRef<Message>().DebugString();
}

void RenderHelper::applyParameter(const ShaderPtr &pShader, const string& paramName, const AutomaticParameter& param) {
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
                        ImagePtr pImage;
                        m_Cache.tryGet<IImageBase>(sourceName, pImage);
                        if (!pImage)
                            throw std::runtime_error(string("no supplied image named : ") + sourceName);
                        imageDescriptionHolder = pImage->getImageDescription();
                        pImageDescription = &imageDescriptionHolder;
                        break;
                    }
                    case SamplingSource::SURFACE: {
                        RenderTargets::const_iterator itr = m_Context.renderTargets.find(sourceName);
                        if (itr != m_Context.renderTargets.end())
                            pImageDescription = &(itr->second.getTexture()->getImageDescription());
                        break;
                    }
                    default:
                        assert(!"not yet implemented");
                }
            }
            data[0] = pImageDescription ? pImageDescription->width : m_Context.renderTargetWidth();
            data[1] = pImageDescription ? pImageDescription->height : m_Context.renderTargetHeight();
            data[2] = data[0] / data[1];
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

void RenderHelper::applyParameter(const ShaderPtr &pShader, const string& paramName, const StaticParameter& param) {
    switch (param.type()) {
        case StaticParameter::FLOAT:
            pShader->setParameter(paramName, param.floatvalue().data(), param.floatvalue_size());
            break;
        case StaticParameter::SAMPLER: {
            assert(param.has_samplingsource());
            const SamplingSource &samplingSource = param.samplingsource();
            const string &sourceName = samplingSource.name();
            ITextureBase* pTexture;
            switch (samplingSource.type()) {
                case SamplingSource::CLIP:
                    m_Context.volatileTextures.push_back(m_TexturePool.get(getImageDescriptionFromClip(sourceName)));
                    pTexture = m_Context.volatileTextures.back().getTexture();
                    break;
                case SamplingSource::SUPPLIED:
                    m_Context.textures.push_back(getNamedTexture(m_Renderer, sourceName));
                    pTexture = m_Context.textures.back().get();
                    break;
                case SamplingSource::SURFACE: {
                    const RenderTargets &targets = m_Context.renderTargets;
                    const RenderTargets::const_iterator itr = targets.find(sourceName);
                    if (itr == targets.end())
                        throw runtime_error("unknown render target '" + sourceName + "' to sample from");
                    pTexture = itr->second.getTexture();
                    break;
                }
                default:
                    cerr << "SamplingSource with type " << SamplingSource_Type_Name(samplingSource.type()) << " is not supported" << endl;
                    return;
            }
            assert(pTexture);
            m_Renderer.setTexture(pShader->getParameter(paramName), param.samplerstate(), pTexture);
            break;
        }
        default: {
            assert(!"not yet implemented");
            break;
        }
    }
}

inline const ImageDescription& RenderHelper::getSafeImageDescription(const ImageDescription* pImage) const {
    return (pImage == NULL || pImage->blank()) ? m_EmptyImageDescription : *pImage;
}

const ImageDescription& RenderHelper::getImageDescriptionFromClip(const string &clipName) const {
    if (clipName.empty())
        return getSafeImageDescription(m_Images.empty() ? NULL : &m_Images[0]);

    for (size_t i = 0; i < m_Clips.size(); ++i) {
        const duke::protocol::Clip &clip = m_Clips[i];
        if (clip.has_name() && clip.name() == clipName)
            return getSafeImageDescription(&m_Images[i]);
    }

    cerr << HEADER + "no clip associated to " << clipName << endl;
    return m_EmptyImageDescription;
}

DukeEngine::PresentStatus DukeEngine::getPresentStatus() {
    return m_Playback.shouldPresent() ? PRESENT_NEXT_BLANKING : SKIP_NEXT_BLANKING;
}

void DukeEngine::waitForBlankingAndWarn(IRenderer& renderer, bool presented) {
    renderer.waitForBlanking();
    verticalBlanking(presented);
}

void DukeEngine::verticalBlanking(bool presented) {
    const boost::chrono::high_resolution_clock::time_point now(playback::s_Clock.now());
    m_VbiTimings.push(now);
    if (presented)
        m_FrameTimings.push(now);
}

void DukeEngine::render(DukeWidget& widget) {
    RenderHelper helper(*this, widget);

    helper.consumeRenderMessages();
    helper.render();
}

