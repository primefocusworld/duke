/*
 * ParameterBuilder.cpp
 *
 *  Created on: 12 juin 2012
 *      Author: Nicolas Rondaud
 */

#include "ParameterBuilder.h"
#include <boost/make_shared.hpp>

namespace duke {
namespace protocol {

const std::string ParameterBuilder::display_dim("displayDim");
const std::string ParameterBuilder::display_mode("displayMode");
const std::string ParameterBuilder::image_dim("imageDim");
const std::string ParameterBuilder::image_ratio("imageRatio");
const std::string ParameterBuilder::zoom_ratio("zoom");
const std::string ParameterBuilder::pan_x("panX");
const std::string ParameterBuilder::pan_y("panY");

StaticParameter ParameterBuilder::zoom(double ratio) {
    return ParameterBuilder::staticFloat(ParameterBuilder::zoom_ratio, ratio);
}

StaticParameter ParameterBuilder::panX(double x) {
    return ParameterBuilder::staticFloat(ParameterBuilder::pan_x, x);
}

StaticParameter ParameterBuilder::panY(double y) {
    return ParameterBuilder::staticFloat(ParameterBuilder::pan_y, y);
}

StaticParameter ParameterBuilder::fitToNormalSize() {
    return ParameterBuilder::staticFloat(ParameterBuilder::display_mode, 0.);
}

StaticParameter ParameterBuilder::fitImageToWindowHeight() {
    return ParameterBuilder::staticFloat(ParameterBuilder::display_mode, 1.);
}

StaticParameter ParameterBuilder::fitImageToWindowWidth() {
    return ParameterBuilder::staticFloat(ParameterBuilder::display_mode, 2.);
}

StaticParameter ParameterBuilder::stretchImageToWindow() {
    return ParameterBuilder::staticFloat(ParameterBuilder::display_mode, 3.);
}

StaticParameter ParameterBuilder::staticFloat(std::string name, float f, std::string scopeName) {
    StaticParameter parameter;
    parameter.set_type(StaticParameter::FLOAT);
    parameter.add_floatvalue(f);
    if (!scopeName.empty())
        name = scopeName + "|" + name;
    parameter.set_name(name);
    return parameter;
}

boost::shared_ptr<google::protobuf::Message> ParameterBuilder::build(const std::string name) {
    return boost::shared_ptr<StaticParameter>();
}

} // namespace protocol
} // namespace duke
