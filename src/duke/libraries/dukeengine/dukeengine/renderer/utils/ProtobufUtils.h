/*
 * ProtobufUtils.h
 *
 *  Created on: Apr 21, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef PROTOBUFUTILS_H_
#define PROTOBUFUTILS_H_

#include <dukeengine/renderer/Enums.h>
#include <player.pb.h>

struct Enums {
    static TPrimitiveType Get(const ::duke::protocol::Mesh_MeshType& type);
    static TPixelFormat Get(const ::duke::protocol::Texture_TextureFormat& format);
    static ::duke::protocol::Texture_TextureFormat Get(const TPixelFormat& format);
};

#endif /* PROTOBUFUTILS_H_ */
