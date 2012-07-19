#ifndef GRADINGBUILDER_H_
#define GRADINGBUILDER_H_

#include "details/IBuilder.h"
#include <player.pb.h>
#include <string>

namespace duke {
namespace protocol {

class GradingBuilder : public IBuilder {

public:
    const static std::string default_grading;
    /*
    static Grading buildRenderPass(RenderPass* const _pPass, std::string _name, std::string _meshName, bool _clean);
    static Grading buildEffect(Effect* const _pEffect, std::string _psName, std::string _vsName);
    static Grading addEffectToRenderPass(RenderPass & _r, std::string _psName, std::string _vsName);
    static Grading addEffectToGrading(Grading & _g, std::string _psName, std::string _vsName, std::string _meshName, bool _clean);
    static Grading addGradingToClip(Clip & _c, std::string _name);
    */

    static void zoom(double ratio);
    static void pan(double x, double y);
    static Grading grading(std::string name, std::string clipName = "");
    virtual boost::shared_ptr<google::protobuf::Message> build(const std::string name);
};

} // namespace protocol
} // namespace duke

#endif // GRADINGBUILDER_H_
