/*
 * GradingBuilder.cpp
 *
 *  Created on: 12 juin 2012
 *      Author: Nicolas Rondaud
 */

#include "GradingBuilder.h"
#include <boost/make_shared.hpp>

namespace duke {
namespace protocol {

const std::string GradingBuilder::default_grading("grading");

/*
Grading GradingBuilder::buildRenderPass(RenderPass* const _pPass, std::string _name, std::string _meshName, bool _clean) {
    _pPass->set_name(_name);
    _pPass->set_clean(_clean);
#ifdef DEBUG
    _pPass->set_cleancolor(0xFF00FF00);
#else
    _pPass->set_cleancolor(0x00000000);
#endif
    _pPass->add_meshname(_meshName);
}

Grading GradingBuilder::buildEffect(Effect* const _pEffect, std::string _psName, std::string _vsName) {
    _pEffect->set_pixelshadername(_psName);
    _pEffect->set_vertexshadername(_vsName);
    _pEffect->set_alphablend(false);
}

Grading GradingBuilder::addEffectToRenderPass(RenderPass & _r, std::string _psName, std::string _vsName) {
    Effect* pEffect = _r.mutable_effect();
    buildEffect(pEffect, _psName, _vsName);
}

Grading GradingBuilder::addEffectToGrading(Grading & _g, std::string _psName, std::string _vsName, std::string _meshName, bool _clean) {
    RenderPass* const pPass = _g.add_pass();
    buildRenderPass(pPass, "pass", _meshName, _clean);
    addEffectToRenderPass(*pPass, _psName, _vsName);
}

Grading GradingBuilder::addGradingToClip(Clip & _c, std::string _name = "grading") {
    if (!_c.has_name()) {
        throw std::runtime_error("Error building Grading: undefined clip name.");
    }
    Grading * const pGrading = _c.mutable_grade();
    buildGrading(*pGrading, _name, "");
    return pGrading;
}
*/

Grading GradingBuilder::grading(std::string name, std::string clipName) {
    Grading grading;
    if (!clipName.empty())
        name += "_" + clipName;
    grading.set_name(name);
    return grading;
}

boost::shared_ptr<google::protobuf::Message> GradingBuilder::build(const std::string name) {
    if (name != GradingBuilder::default_grading)
        return boost::shared_ptr<Grading>();
    return boost::make_shared<Grading>(grading(GradingBuilder::default_grading));
}

} // namespace protocol
} // namespace duke
