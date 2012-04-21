#ifndef MESHFACTORY_H_
#define MESHFACTORY_H_

#include <dukeengine/renderer/resource/IMeshBase.h>

namespace duke {namespace protocol {class Mesh;}}

MeshPtr build(IRenderer& renderer, const ::duke::protocol::Mesh& mesh);

#endif /* MESHFACTORY_H_ */
