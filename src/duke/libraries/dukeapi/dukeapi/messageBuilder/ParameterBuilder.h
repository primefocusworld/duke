#ifndef PARAMETERBUILDER_H_
#define PARAMETERBUILDER_H_

#include <dukeapi/IMessageIO.h>
#include <player.pb.h>
#include <iostream>
#include <stdexcept>

namespace dk = ::duke::protocol;

static inline void buildStaticFloatParam(dk::StaticParameter & _sp //
                           , std::string _name //
                           , float _f) {
    _sp.set_name(_name);
    _sp.set_type(dk::StaticParameter::FLOAT);
    _sp.add_floatvalue(_f);
}

static inline void buildAutomaticParam(dk::AutomaticParameter & _ap //
                         , std::string _name //
                         , dk::AutomaticParameter_Type _type = dk::AutomaticParameter::FLOAT3_TEX_DIM) {

    _ap.set_name(_name);
    _ap.set_type(_type);
    return;
}

static inline void buildAutomaticClipSourceParam(dk::AutomaticParameter & _ap //
                                   , std::string _name //
                                   , std::string _clipName) {

    if (_clipName == "")
        throw std::runtime_error("Error building AutomaticParameter: invalid clip name.");

    _name = _clipName + "|" + _name;
    _ap.set_name(_name);
    _ap.set_type(dk::AutomaticParameter::FLOAT3_TEX_DIM);

    dk::SamplingSource *pSource = _ap.mutable_samplingsource();
    pSource->set_type(dk::SamplingSource::CLIP);
    pSource->set_name(_clipName);
}

static inline void buildStaticSamplerParam(dk::StaticParameter & _sp //
                             , std::string _name //
                             , std::string _srcName //
                             , dk::SamplingSource_Type _type = dk::SamplingSource::CLIP //
                             , std::string _scopeName = "") {

    // parameters nomenclature: clip_name | param_name
    if (_srcName == "")
        throw std::runtime_error("Error building StaticParameter: Sampler: invalid source name.");

    if ((_scopeName == "") && (_type == dk::SamplingSource::CLIP)) {
        _scopeName = _srcName;
    }

    if (_scopeName != "")
        _name = _scopeName + "|" + _name;

    _sp.set_name(_name);
    _sp.set_type(dk::StaticParameter::SAMPLER);
    // ...sampling source
    dk::SamplingSource* pSamplingSource = _sp.mutable_samplingsource();
    pSamplingSource->set_type(_type);
    pSamplingSource->set_name(_srcName);
    // ...sampler state
    dk::SamplerState* pSamplerState = NULL;
    pSamplerState = _sp.add_samplerstate();
    pSamplerState->set_type(dk::SamplerState::MIN_FILTER);
    pSamplerState->set_value(dk::SamplerState::TEXF_POINT);
    pSamplerState = _sp.add_samplerstate();
    pSamplerState->set_type(dk::SamplerState::MAG_FILTER);
    pSamplerState->set_value(dk::SamplerState::TEXF_POINT);
    pSamplerState = _sp.add_samplerstate();
    pSamplerState->set_type(dk::SamplerState::WRAP_S);
    pSamplerState->set_value(dk::SamplerState::WRAP_BORDER);
    pSamplerState = _sp.add_samplerstate();
    pSamplerState->set_type(dk::SamplerState::WRAP_T);
    pSamplerState->set_value(dk::SamplerState::WRAP_BORDER);
}

static inline void addStaticFloatParam(IMessageIO & _queue //
                         , std::string _name //
                         , float _f //
                         , std::string _scopeName = "") {

    if (_scopeName != "")
        _name = _scopeName + "|" + _name;

    dk::StaticParameter p;
    buildStaticFloatParam(p, _name, _f);
    push(_queue, p);
}

static inline void addStaticSamplerParam(IMessageIO & _queue //
                           , std::string _name //
                           , std::string _srcName //
                           , dk::SamplingSource_Type _type = dk::SamplingSource::CLIP //
                           , std::string _scopeName = "") {

    dk::StaticParameter p;
    buildStaticSamplerParam(p, _name, _srcName, _type, _scopeName);
    push(_queue, p);
}

static inline void addAutomaticParam(IMessageIO & _queue //
                       , std::string _name //
                       , dk::AutomaticParameter_Type _type = dk::AutomaticParameter::FLOAT3_TEX_DIM) {

    dk::AutomaticParameter p;
    buildAutomaticParam(p, _name, _type);
    push(_queue, p);
}

static inline void addAutomaticClipSourceParam(IMessageIO & _queue //
                                 , std::string _name //
                                 , std::string _clipName) {

    dk::AutomaticParameter p;
    buildAutomaticClipSourceParam(p, _name, _clipName);
    push(_queue, p);
}

#endif /* PARAMETERBUILDER_H_ */
