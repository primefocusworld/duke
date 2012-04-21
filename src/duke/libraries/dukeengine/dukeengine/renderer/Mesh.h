#ifndef MESH_H_
#define MESH_H_

#include <player.pb.h>
#include <boost/shared_ptr.hpp>

class IRenderer;
class IMeshBase;

class Mesh {
public:
    Mesh(IRenderer&, const ::duke::protocol::Mesh&);
    virtual ~Mesh();

    void render(IRenderer&) const;

private:
    ::boost::shared_ptr<IMeshBase> m_pMeshBase;
};

#endif /* MESH_H_ */
