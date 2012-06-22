#ifndef PARAMETERBUILDER_H_
#define PARAMETERBUILDER_H_

#include "details/IBuilder.h"
#include <player.pb.h>
#include <string>

namespace duke {
namespace protocol {

class ParameterBuilder : public IBuilder {
public:
    const static std::string display_dim;
    const static std::string display_mode;
    const static std::string image_dim;
    const static std::string image_ratio;
    const static std::string zoom_ratio;
    const static std::string pan_x;
    const static std::string pan_y;

    static StaticParameter zoom(double ratio);
    static StaticParameter panX(double x);
    static StaticParameter panY(double y);
    static StaticParameter fitToNormalSize();
    static StaticParameter fitImageToWindowHeight();
    static StaticParameter fitImageToWindowWidth();
    static StaticParameter stretchImageToWindow();
    static StaticParameter staticFloat(std::string name, float f, std::string scopeName = "");

    virtual boost::shared_ptr<google::protobuf::Message> build(const std::string name);
};

} // namespace protocol
} // namespace duke

#endif // PARAMETERBUILDER_H_
