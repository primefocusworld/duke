#ifndef SHADERFACTORY_H_
#define SHADERFACTORY_H_

#include "Enums.h"
#include "ResourceCache.h"
#include "IShaderBase.h"
#include "RenderingContext.h"
#include "ProtoBufResource.h"
#include <player.pb.h>
#include <dukeio/ImageDescription.h>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

ShaderBasePtr compile(IRenderer& renderer, const ::duke::protocol::Shader& shader, const TShaderType type);

#endif /* SHADERFACTORY_H_ */
