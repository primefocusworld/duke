#ifndef MESHFACTORY_H_
#define MESHFACTORY_H_

#include <dukeengine/renderer/resource/IMeshBase.h>

namespace duke {namespace protocol {class Mesh;}}

IMeshBase* buildMesh(IRenderer& renderer, const ::duke::protocol::Mesh& mesh);

#endif /* MESHFACTORY_H_ */
